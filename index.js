const addon = require('./build/Release/ration_calculator');

const idealNutrition = {
  calories: 1300,
  proteins: 1300 * 0.27,
  carbs: 1300 * 0.47,
  fats: 1300 * 0.26
};

const foodsArr = [
  {
    food: {
      id: "food",
      nutrition: {
        proteins: 20,
        carbs: 30,
        fats: 40,
        calories: 212,
      },
      glycemicIndex: 45
    },
    available: 50,
    delta: 7,
    daily: {
      max: 100,
      min: 10
    }
  }
];

let calculateRation = (idealNutrition, foodsArr) => {
  return new Promise((rslv, rjct) => {
    try {
      addon.createFoodPlan(idealNutrition, foodsArr, function(error, nutrition, ration) {
        rslv({ error, nutrition, ration });
      }); 
    } catch (error) {
      console.error(error);
      rjct(error);
    }
  });
}

calculateRation(idealNutrition, foodsArr)
.then(res => {
  console.log(res.error);
  console.log(res.nutrition);
  console.log(res.ration);
})
.catch(err => {
  console.log(err);
});