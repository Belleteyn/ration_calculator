const addon = require('./build/Release/ration_calculator');

exports.calculateRation = (idealNutrition, foodsArr) => {
  return new Promise((rslv, rjct) => {
    try {
      let res = addon.createFoodPlan(idealNutrition, foodsArr);
      rslv(res); 
    } catch (error) {
      console.error(error);
      rjct(error);
    }
  });
}