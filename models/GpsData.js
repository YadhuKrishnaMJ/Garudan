const mongoose = require('mongoose'); //Mongoose Module for MongoDB Connection

//New Schema for gps data storage in MongoDB
const gpsSchema = new mongoose.Schema({ 
  latitude: Number,      
  longitude: Number,
  temperatureC: Number,     // temperature in Celsius
  temperatureF: Number,     // temperature in Fahrenheit
  pressure: Number,         // pressure
  timestamp: { type: Date, default: Date.now }    //Timestamp to access most recent and past location data
});

module.exports = mongoose.model('GpsData', gpsSchema);
