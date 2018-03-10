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

try {
  addon.createFoodPlan(idealNutrition, foodsArr, function(error, nutrition, ration) {
    console.log(`error: ${JSON.stringify(error)}`);
    console.log(`nutrition: ${JSON.stringify(nutrition)}`);
    console.log(`ration: ${JSON.stringify(ration)}`);
  }); 
} catch (error) {
  console.error(error);
}
