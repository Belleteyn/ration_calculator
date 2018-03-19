#include <node.h>
#include <v8.h>

#include <map>
#include <iostream>

#include "NutritionModuleCppSrc/NutritionModule.h"

using namespace v8;

using GIPair = std::pair<int16_t, FoodAvailable>;
using GIMap = std::multimap<int16_t, FoodAvailable>;

/// Stringify V8 value to JSON
/// return empty string for empty value
std::string json_str(v8::Isolate* isolate, v8::Handle<v8::Value> value)
{
  if (value.IsEmpty())
  {
    return std::string();
  }

  v8::HandleScope scope(isolate);

  v8::Local<v8::Object> json = isolate->GetCurrentContext()->
      Global()->Get(v8::String::NewFromUtf8(isolate, "JSON"))->ToObject();
  v8::Local<v8::Function> stringify = json->Get(v8::String::NewFromUtf8(isolate, "stringify")).As<v8::Function>();

  v8::Local<v8::Value> result = stringify->Call(json, 1, &value);
  v8::String::Utf8Value const str(result);

  return std::string(*str, str.length());
}

Nutrition parse(Isolate* isolate, const Local<Object>& obj) {
  Local<Context> context = isolate->GetCurrentContext();

  const Local<String> caloriesKey = String::NewFromUtf8(isolate, "calories");
  const Local<String> proteinsKey = String::NewFromUtf8(isolate, "proteins");
  const Local<String> carbsKey = String::NewFromUtf8(isolate, "carbs");
  const Local<String> fatsKey = String::NewFromUtf8(isolate, "fats");
  const Local<String> totalKey = String::NewFromUtf8(isolate, "total");
  
  Local<Object> caloriesObj = Local<Object>::Cast(obj->Get(context, caloriesKey).ToLocalChecked());
  Local<Object> carbsObj = Local<Object>::Cast(obj->Get(context, carbsKey).ToLocalChecked());
  Local<Object> fatsObj = Local<Object>::Cast(obj->Get(context, fatsKey).ToLocalChecked());
  
  int calories = 0, proteins = 0, fats = 0, carbs = 0;  
  if (!caloriesObj->IsUndefined()) {
    Local<Value> caloriesVal = caloriesObj->Get(context, totalKey).ToLocalChecked();
    if (caloriesVal->IsNumber())
      calories = caloriesVal->NumberValue();
  }
  
  if (!carbsObj->IsUndefined()) {
    Local<Value> carbsVal = carbsObj->Get(context, totalKey).ToLocalChecked();
    if (carbsVal->IsNumber())
      carbs = carbsVal->NumberValue();
  }
  
  if (!fatsObj->IsUndefined()) {
    Local<Value> fatsVal = fatsObj->Get(context, totalKey).ToLocalChecked();
    if (fatsVal->IsNumber())
      fats = fatsVal->NumberValue();
  }

  Local<Value> proteinsVal = obj->Get(context, proteinsKey).ToLocalChecked();
  if (proteinsVal->IsNumber())
    proteins = proteinsVal->NumberValue();
  
  return Nutrition(calories, proteins, carbs, fats);
}

namespace GIMapPrivate {

  FoodAvailable::Daily parseDaily(Isolate* isolate, const Local<Object>& obj, const int maxAvail)
  {
    if (obj.IsEmpty() || obj->IsUndefined())
    {
      return FoodAvailable::Daily(maxAvail, 0, 0);
    }

    const Local<String> maxKey = String::NewFromUtf8(isolate, "max");
    const Local<String> minKey = String::NewFromUtf8(isolate, "min");
    
    Local<Context> context = isolate->GetCurrentContext();
    Local<Value> maxVal = obj->Get(context, maxKey).ToLocalChecked();
    Local<Value> minVal = obj->Get(context, minKey).ToLocalChecked();

    int min = 0, max = maxAvail;
    if (minVal->IsNumber())
      min = minVal->NumberValue();

    if (maxVal->IsNumber())
      max = maxVal->NumberValue();

    return FoodAvailable::Daily(max, 0, min);
  }

  GIMap parse(Isolate* isolate, const Local<Array>& array) 
  {
    GIMap map;
    
    size_t arrLength = array->Length();

    const Local<String> availKey = String::NewFromUtf8(isolate, "available");
    const Local<String> deltaKey = String::NewFromUtf8(isolate, "delta");
    const Local<String> dailyKey = String::NewFromUtf8(isolate, "daily");
    const Local<String> foodKey = String::NewFromUtf8(isolate, "food");
    
    const Local<String> giKey = String::NewFromUtf8(isolate, "glycemicIndex");
    const Local<String> idKey = String::NewFromUtf8(isolate, "_id");
    const Local<String> nutritionKey = String::NewFromUtf8(isolate, "nutrition");

    Local<Context> context = isolate->GetCurrentContext();

    for (unsigned int i = 0; i < arrLength; i++ ) {
      try {  
        Local<Object> v = Local<Object>::Cast(array->Get(i));
        
        Local<Object> dailyVal = Local<Object>::Cast(v->Get(context, dailyKey).ToLocalChecked());
        Local<Object> foodVal = Local<Object>::Cast(v->Get(context, foodKey).ToLocalChecked());

        if (foodVal.IsEmpty() || foodVal->IsUndefined() || foodVal->IsNull()) {
          std::cout << "Error on parsing food data: empty food\n";
          continue;
        }

        Local<Value> availVal = v->Get(context, availKey).ToLocalChecked();
        Local<Value> deltaVal = v->Get(context, deltaKey).ToLocalChecked();
        Local<Value> giVal = foodVal->Get(context, giKey).ToLocalChecked();
        Local<Value> idVal = foodVal->Get(context, idKey).ToLocalChecked();
        Local<Object> nutritionObj = Local<Object>::Cast(foodVal->Get(context, nutritionKey).ToLocalChecked());

        if (nutritionObj.IsEmpty() || nutritionObj->IsUndefined() || nutritionObj->IsNull()) {
          std::cout << "Error on parsing food data: no nutrition\n";
          continue;
        }
        
        std::string idStr(*String::Utf8Value(idVal));
        if (idStr.empty()) {
          std::cout << "Error on parsing food data: no food id\n";
          continue;
        }

        Nutrition nutrition = ::parse(isolate, nutritionObj);

        int gi = 0, avail = 0, delta = 0;
        if (giVal->IsNumber())
          gi = giVal->NumberValue();
        
        if (availVal->IsNumber())
          avail = availVal->NumberValue();
          
        if (deltaVal->IsNumber())
          delta = deltaVal->NumberValue();

        GIPair pair = GIPair(gi
          , FoodAvailable(Food(idStr, nutrition.proteins, nutrition.carbs, nutrition.fats, nutrition.kkal)
            , avail, delta
            , parseDaily(isolate, dailyVal, avail)
          )
        );

        map.insert(pair);
      }
      catch(const std::exception& ex) {
        std::cout << "GIMap parse exception: " << ex.what() << std::endl;
      }
    }

    return map;
  }
}

Local<Value> createRationObj(Isolate* isolate, const char* foodId
  , unsigned int portionMass, unsigned int delta)
{
  EscapableHandleScope scope(isolate);
  Local<Object> temp = Object::New(isolate);
  temp->Set(String::NewFromUtf8(isolate, "food"), String::NewFromUtf8(isolate, foodId));
  temp->Set(String::NewFromUtf8(isolate, "size"), Number::New(isolate, portionMass));
  temp->Set(String::NewFromUtf8(isolate, "delta"), Number::New(isolate, delta));
  return scope.Escape(temp);
}

void CreateFoodPlan(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope handle_scope(isolate);

  if (args.Length() != 2) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsObject() || !args[1]->IsArray()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong type of arguments")));
    return;
  }

  Local<Object> idealNutritionObj = Local<Object>::Cast(args[0]);
  Local<Array> foodsArr = Local<Array>::Cast(args[1]);
  
  static const Nutrition allowedNutritionOverheading(0.5, 0.5, 0.3, 0.1);
  const Nutrition idealNutrition = parse(isolate, idealNutritionObj);
  const GIMap giMap = GIMapPrivate::parse(isolate, foodsArr);
  
  Local<Object> ret = Object::New(isolate);
  Local<Object> Error = Object::New(isolate);
  Local<Object> NutritionObj = Object::New(isolate);
  Local<Array> Ration = Array::New(isolate);

  auto callback = [&idealNutrition, &isolate, &Error, &NutritionObj, &Ration](const FoodTree::Ration& ration
    , const Nutrition& nutrition, const NutritionError& error) 
  {  
    std::cout << "\nration: " << ration.size() << " foods \n\n";
    for (auto iter = ration.begin(); iter != ration.end(); ++iter)
    {
      std::cout << (*iter)->getName() << " -> " << (*iter)->getPortionMass() << std::endl;
    }

    std::cout << "\nSummary: Error = " << error.error() * 100 << std::endl;

    std::cout << "kkal: " << nutrition.kkal << "(" << idealNutrition.kkal << ")"
              << ", kkal error = " << error.kkalErr;

    std::cout << "\np: " << nutrition.proteins << "(" << idealNutrition.proteins << ")"
              << ", error = " << error.proteinsErr;

    std::cout << "\nc: " << nutrition.carbs << "(" << idealNutrition.carbs << ")"
              << ", error = " << error.carbsErr;

    std::cout << "\nf: " << nutrition.fats << "(" << idealNutrition.fats << ")"
              << ", error = " << error.fatsErr << std::endl << std::endl;

    int i = 0;
    for (auto iter = ration.begin(); iter != ration.end(); ++iter)
    {
      Ration->Set(i, createRationObj(isolate, (*iter)->getName()
        , (*iter)->getPortionMass(), (*iter)->getDelta()));
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
    Local<Object> ret = Object::New(isolate);
    ret->Set(String::NewFromUtf8(isolate, "error"), Error);
    ret->Set(String::NewFromUtf8(isolate, "nutrition"), NutritionObj);
    ret->Set(String::NewFromUtf8(isolate, "ration"), Ration);

    args.GetReturnValue().Set(ret);
  }
  else {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "ration creatoin failed: no variants")));
  }
}

void Init(Handle<Object> exports) {
  NODE_SET_METHOD(exports, "createFoodPlan", CreateFoodPlan);
}

NODE_MODULE(hello, Init)