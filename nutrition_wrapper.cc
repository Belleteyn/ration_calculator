#include <node.h>
#include <v8.h>

#include <map>
#include <iostream>

#include "NutritionModuleCppSrc/NutritionModule.h"

using namespace v8;

using GIPair = std::pair<int16_t, FoodAvailable>;
using GIMap = std::multimap<int16_t, FoodAvailable>;

Nutrition parse(const Local<Object>& obj, Isolate* isolate, const Local<Context>& context) {
  static const Local<String> caloriesKey = String::NewFromUtf8(isolate, "calories");
  static const Local<String> proteinsKey = String::NewFromUtf8(isolate, "proteins");
  static const Local<String> carbsKey = String::NewFromUtf8(isolate, "carbs");
  static const Local<String> fatsKey = String::NewFromUtf8(isolate, "fats");
  
  Local<Value> caloriesVal = obj->Get(context, caloriesKey).ToLocalChecked();
  Local<Value> proteinsVal = obj->Get(context, proteinsKey).ToLocalChecked();
  Local<Value> carbsVal = obj->Get(context, carbsKey).ToLocalChecked();
  Local<Value> fatsVal = obj->Get(context, fatsKey).ToLocalChecked();

  return Nutrition(caloriesVal->NumberValue(), proteinsVal->NumberValue(), carbsVal->NumberValue(), fatsVal->NumberValue());
}

namespace GIMapPrivate {

  FoodAvailable::Daily parseDaily(const Local<Object>& obj, Isolate* isolate, const Local<Context>& context)
  {
    static const Local<String> maxKey = String::NewFromUtf8(isolate, "max");
    static const Local<String> minKey = String::NewFromUtf8(isolate, "min");
    
    Local<Value> maxVal = obj->Get(context, maxKey).ToLocalChecked();
    Local<Value> minVal = obj->Get(context, minKey).ToLocalChecked();

    return FoodAvailable::Daily(maxVal->NumberValue(), 0, minVal->NumberValue());
  }

  Food parseFood(const Local<Object>& obj, Isolate* isolate, const Local<Context>& context)
  {
    static const Local<String> idKey = String::NewFromUtf8(isolate, "_id");
    static const Local<String> nutritionKey = String::NewFromUtf8(isolate, "nutrition");
    
    Local<Value> idVal = obj->Get(context, idKey).ToLocalChecked();
    Local<Object> nutritionObj = obj->Get(context, nutritionKey).ToLocalChecked()->ToObject();
    
    std::string idStr(*String::Utf8Value(idVal));
    Nutrition nutrition = parse(nutritionObj, isolate, context);

    return Food(idStr, nutrition.proteins, nutrition.carbs, nutrition.fats, nutrition.kkal);
  }

  GIMap parse(const Local<Array>& array, Isolate* isolate, const Local<Context>& context) 
  {
    GIMap map;
    
    size_t arrLength = array->Length();

    static const Local<String> availKey = String::NewFromUtf8(isolate, "available");
    static const Local<String> deltaKey = String::NewFromUtf8(isolate, "delta");
    static const Local<String> dailyKey = String::NewFromUtf8(isolate, "daily");
    static const Local<String> foodKey = String::NewFromUtf8(isolate, "food");
    static const Local<String> giKey = String::NewFromUtf8(isolate, "glycemicIndex");
    
    for (unsigned int i = 0; i < arrLength; i++ ) {
      Local<Object> v = array->Get(i)->ToObject();
      
      Local<Value> availVal = v->Get(context, availKey).ToLocalChecked();
      Local<Value> deltaVal = v->Get(context, deltaKey).ToLocalChecked();
      Local<Object> dailyVal = v->Get(context, dailyKey).ToLocalChecked()->ToObject();
      Local<Object> foodVal = v->Get(context, foodKey).ToLocalChecked()->ToObject();
      Local<Value> giVal = foodVal->Get(context, giKey).ToLocalChecked();

      GIPair pair = GIPair(giVal->NumberValue()
        , FoodAvailable(parseFood(foodVal, isolate, context)
        , availVal->NumberValue(), deltaVal->NumberValue()
        , parseDaily(dailyVal, isolate, context)));

      map.insert(pair);
    }

    return map;
  }
}

void CreateFoodPlan(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  
  if (args.Length() != 3) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsObject() || !args[1]->IsArray() || !args[2]->IsFunction()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong type of arguments")));
    return;
  }

  HandleScope handle_scope(isolate);
  
  Local<Context> context = isolate->GetCurrentContext();
  Local<Object> idealNutritionObj = args[0]->ToObject(context).ToLocalChecked();
  Local<Array> foodsArr = Local<Array>::Cast(args[1]);

  static const Nutrition allowedNutritionOverheading(0.5, 0.5, 0.3, 0.1);
  const Nutrition idealNutrition = parse(idealNutritionObj, isolate, context);
  const GIMap giMap = GIMapPrivate::parse(foodsArr, isolate, context);

  Local<Function> cb = Local<Function>::Cast(args[2]);
  Local<Object> Error = Object::New(isolate);
  Local<Object> NutritionObj = Object::New(isolate);
  Local<Array> Ration = Array::New(isolate);

  static auto callback = [&idealNutrition, &isolate, &Error, &NutritionObj, &Ration](const FoodTree::Ration& ration
    , const Nutrition& nutrition, const NutritionError& error) 
  {  
    int i = 0;
    for (auto iter = ration.begin(); iter != ration.end(); ++iter)
    {
      Local<Object> temp = Object::New(isolate);
      
      temp->Set(String::NewFromUtf8(isolate, "food"), String::NewFromUtf8(isolate, (*iter)->getName()));
      temp->Set(String::NewFromUtf8(isolate, "size"), Number::New(isolate, (*iter)->getPortionMass()));

      Ration->Set(i, temp);
      i++;
    }

    Error->Set(String::NewFromUtf8(isolate, "caloriesError"), Number::New(isolate, error.kkalErr));
    Error->Set(String::NewFromUtf8(isolate, "proteinsError"), Number::New(isolate, error.proteinsErr));
    Error->Set(String::NewFromUtf8(isolate, "carbsError"), Number::New(isolate, error.carbsErr));
    Error->Set(String::NewFromUtf8(isolate, "fatsError"), Number::New(isolate, error.fatsErr));

    NutritionObj->Set(String::NewFromUtf8(isolate, "calories"), Number::New(isolate, nutrition.kkal));
    NutritionObj->Set(String::NewFromUtf8(isolate, "proteins"), Number::New(isolate, nutrition.proteins));
    NutritionObj->Set(String::NewFromUtf8(isolate, "carbs"), Number::New(isolate, nutrition.carbs));
    NutritionObj->Set(String::NewFromUtf8(isolate, "fats"), Number::New(isolate, nutrition.fats));
  };

  bool res = NutritionAnalyzer::createDailyNutritionPlan(giMap, idealNutrition, allowedNutritionOverheading, callback);
  if (res) {
    const unsigned argc = 3;
    Local<Value> argv[argc] = { Error, NutritionObj, Ration };
    cb->Call(isolate->GetCurrentContext()->Global(), argc, argv);
  }
  else {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "error")));
  }
}

void Init(Handle<Object> exports) {
  NODE_SET_METHOD(exports, "createFoodPlan", CreateFoodPlan);
}

NODE_MODULE(hello, Init)