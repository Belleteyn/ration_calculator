const addon = require('./build/Release/ration_calculator');

const idealNutrition = {
  calories: {
    total: 1300
  },
  proteins: 1300 * 0.27,
  carbs: {
    total: 1300 * 0.47
  },
  fats: {
    total: 1300 * 0.26
  }
};

const foodsArr = [ 
  { 
    food: { 
      _id: '5a6ccab519b70038ec83b891',
      glycemicIndex: 25,
      nutrition: {
          "sodium" : 0,
          "potassium" : 136.1,
          "cholesterol" : 0,
          "calories" : {
              "total" : 42.6,
              "fromFat" : 1.2
          },
          "proteins" : 0.8,
          "carbs" : {
              "dietaryFiber" : 1.6,
              "total" : 10.7,
              "sugars" : 7
          },
          "fats" : {
              "total" : 0.1,
              "saturated" : 0,
              "trans" : 0,
              "polyunsaturated" : 0,
              "monounsaturated" : 0
          },
          "vitaminA" : 23,
          "vitaminC" : 52.5,
          "calcium" : 2.2,
          "iron" : 0.5
      }
    },
    available: 140,
    delta: 70,
    daily: { min: 70, max: 140 } 
  },
  { 
    food: { 
      _id: '5a6e114c1940f73e6d61818f',
      name: 'Parmesan cheese',
      glycemicIndex: 0,
      food_icon: 'content_item_cheese',
      "nutrition" : {
          "sodium" : 1800,
          "potassium" : 180,
          "cholesterol" : 86,
          "calories" : {
              "total" : 420,
              "fromFat" : 260
          },
          "proteins" : 28,
          "carbs" : {
              "dietaryFiber" : 0,
              "total" : 14,
              "sugars" : 0
          },
          "fats" : {
              "total" : 28,
              "saturated" : 15.4,
              "trans" : 0.8,
              "polyunsaturated" : 1.4,
              "monounsaturated" : 7.2
          },
          "vitaminA" : 19.4,
          "vitaminC" : 0,
          "calcium" : 86,
          "iron" : 2.8
      },
      piecesPerPack: 1,
      gramsPerPiece: 120 
    },
    available: 200,
    delta: 10,
    daily: { min: 10, max: 50 } 
  },
  { 
    food: { 
      _id: '5a75c1bace98d12dddc63602',
      name: 'Pangasius',
      glycemicIndex: 0,
      "nutrition" : {
          "sodium" : 118.8,
          "potassium" : 365.6,
          "cholesterol" : 65.7,
          "calories" : {
              "total" : 144,
              "fromFat" : 65
          },
          "proteins" : 18.2,
          "carbs" : {
              "dietaryFiber" : 0,
              "total" : 0,
              "sugars" : 0
          },
          "fats" : {
              "total" : 7,
              "saturated" : 1.6,
              "trans" : 0.1,
              "polyunsaturated" : 1.3,
              "monounsaturated" : 3.1
          },
          "vitaminA" : 0,
          "vitaminC" : 0,
          "calcium" : 0.9,
          "iron" : 1.5
      } 
    },
    available: undefined,
    delta: 130,
    daily: { min: null } 
  },
  { food:
    { _id: '5a75b8ee855c912b1615274f',
      name: 'Apple, Granny Smith',
      glycemicIndex: 44,
      "nutrition" : {
          "sodium" : 0,
          "potassium" : 0,
          "cholesterol" : 0,
          "calories" : {
              "total" : 52,
              "fromFat" : 0
          },
          "proteins" : 0,
          "carbs" : {
              "dietaryFiber" : 2.6,
              "total" : 14.3,
              "sugars" : 10.4
          },
          "fats" : {
              "total" : 0,
              "saturated" : 0,
              "trans" : 0,
              "polyunsaturated" : 0,
              "monounsaturated" : 0
          },
          "vitaminA" : 1.3,
          "vitaminC" : 7.8,
          "calcium" : 0,
          "iron" : 1.3
      }
    },
   available: 525,
   delta: 175,
   daily: { min: 175, max: 175 } 
  },
  { food:
    { _id: '5aa4e9a5d3089f2de518dfd8',
      name: 'Очень важная корова, творог обезжиренный',
      glycemicIndex: 0,
      "nutrition" : {
          "calories" : {
              "total" : 85,
              "fromFat" : 0
          },
          "proteins" : 18,
          "carbs" : {
              "total" : 3.3
          },
          "fats" : {
              "total" : 0,
              "saturated" : 0,
              "trans" : 0,
              "polyunsaturated" : 0,
              "monounsaturated" : 0
          }
      }
    },
   available: 220,
   delta: 110
  },
  { food:
    { _id: '5aa4e9f7d3089f2de518dfff',
      name: 'Домик в деревне, творог мягкий обезжиренный',
      glycemicIndex: 0,
      "nutrition" : {
          "calories" : {
              "total" : 61,
              "fromFat" : 0
          },
          "proteins" : 10.9,
          "carbs" : {
              "total" : 4
          },
          "fats" : {
              "total" : 0.1,
              "saturated" : 0,
              "trans" : 0,
              "polyunsaturated" : 0,
              "monounsaturated" : 0
          }
      }
    },
   available: 200,
   delta: 100
  },
  { food:
    { _id: '5aa4eb11d3089f2de518e07f',
      name: 'куриная грудка',
      glycemicIndex: 0,
      "nutrition" : {
          "sodium" : 393,
          "potassium" : 243,
          "cholesterol" : 83,
          "calories" : {
              "total" : 195,
              "fromFat" : 72.15
          },
          "proteins" : 29.55,
          "carbs" : {
              "dietaryFiber" : 0,
              "total" : 0,
              "sugars" : 0
          },
          "fats" : {
              "total" : 7.72,
              "saturated" : 2.172,
              "trans" : 0,
              "polyunsaturated" : 1.646,
              "monounsaturated" : 3.0005
          }
      } 
    },
   available: 540,
   delta: 30
  }
];

const brokenFoodsArr = [
  { //no food
    available: 540,
    delta: 30
  },
  { //no food::gi
    food:
    { 
      _id: '5a75b8ee855c912b1615274f',
      "nutrition" : {
          "sodium" : 0,
          "potassium" : 0,
          "cholesterol" : 0,
          "calories" : {
              "total" : 52,
              "fromFat" : 0
          },
          "proteins" : 0,
          "carbs" : {
              "dietaryFiber" : 2.6,
              "total" : 14.3,
              "sugars" : 10.4
          },
          "fats" : {
              "total" : 0,
              "saturated" : 0,
              "trans" : 0,
              "polyunsaturated" : 0,
              "monounsaturated" : 0
          },
          "vitaminA" : 1.3,
          "vitaminC" : 7.8,
          "calcium" : 0,
          "iron" : 1.3
      }
    },
    available: 525,
    delta: 175,
    daily: { min: 175, max: 175 } 
  },
  { //no food::nutrition
    food: { 
      _id: '5a75c1bace98d12dddc63602',
      name: 'Pangasius',
      glycemicIndex: 0
    },
    available: undefined,
    delta: 130,
    daily: { min: null } 
  },
  { //no food::_id
    food: { 
      name: 'Pangasius',
      glycemicIndex: 0,
      nutrition: {}
    },
    available: undefined,
    delta: 130,
    daily: { min: null } 
  },
  { //no daily
    food:
    { _id: '5aa4eb11d3089f2de518e07f',
      glycemicIndex: 0,
      "nutrition" : {
          "sodium" : 393,
          "potassium" : 243,
          "cholesterol" : 83,
          "calories" : {
              "total" : 195,
              "fromFat" : 72.15
          },
          "proteins" : 29.55,
          "carbs" : {
              "dietaryFiber" : 0,
              "total" : 0,
              "sugars" : 0
          },
          "fats" : {
              "total" : 7.72,
              "saturated" : 2.172,
              "trans" : 0,
              "polyunsaturated" : 1.646,
              "monounsaturated" : 3.0005
          }
      } 
    },
    available: 540,
    delta: 30
  },
  { //no delta
    food:
    { 
      _id: '5aa4e9f7d3089f2de518dfff',
      name: 'Домик в деревне, творог мягкий обезжиренный',
      glycemicIndex: 0,
      "nutrition" : {
          "calories" : {
              "total" : 61,
              "fromFat" : 0
          },
          "proteins" : 10.9,
          "carbs" : {
              "total" : 4
          },
          "fats" : {
              "total" : 0.1,
              "saturated" : 0,
              "trans" : 0,
              "polyunsaturated" : 0,
              "monounsaturated" : 0
          }
      }
    },
    available: 200
  },
  { //no available
    food:
    { _id: '5aa4e9a5d3089f2de518dfd8',
      name: 'Очень важная корова, творог обезжиренный',
      glycemicIndex: 0,
      "nutrition" : {
          "calories" : {
              "total" : 85,
              "fromFat" : 0
          },
          "proteins" : 18,
          "carbs" : {
              "total" : 3.3
          },
          "fats" : {
              "total" : 0,
              "saturated" : 0,
              "trans" : 0,
              "polyunsaturated" : 0,
              "monounsaturated" : 0
          }
      }
    },
    delta: 110
  },
];

let calculateRation = (idealNutrition, foodsArr) => {
  return new Promise((rslv, rjct) => {
    try {
      let res = addon.createFoodPlan(idealNutrition, foodsArr);
      console.log(res);
      rslv(res); 
    } catch (error) {
      console.error(error);
      rjct(error);
    }
  });
}

calculateRation(idealNutrition, brokenFoodsArr)
.then(res => {
  console.log(res);
})
.catch(err => {
  console.log(err);
});