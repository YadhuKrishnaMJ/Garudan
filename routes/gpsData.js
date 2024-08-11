const express = require('express');       // Express Module for creating express app
const router = express.Router();          // Express Router for routing
const GpsData = require('../models/GpsData'); // Gps data model in MongoDB

// Handle POST method for ESP32 to send new location data to the MongoDB Atlas database (Local)
router.post('/gps-data', async (req, res) => {
    const { latitude, longitude, temperatureC, temperatureF, pressure } = req.body; // Acquire latitude, longitude, temperatureC, temperatureF, and pressure from request body
    const newGpsData = new GpsData({ latitude, longitude, temperatureC, temperatureF, pressure });

    try {
        await newGpsData.save(); // Save the newly acquired data as a document
        res.status(200).send('Data saved successfully');
    } catch (err) {
        console.error(err);
        res.status(500).send(err);
    }
});

// GET method for the frontend to update the map with the latest location data
router.get('/latest-gps-data', async (req, res) => {
    try {
        const latestData = await GpsData.findOne().sort({ timestamp: -1 }).exec(); // Sort by timestamp to get the latest location document
        res.json(latestData);
    } catch (err) {
        console.error(err);   
        res.status(500).send(err);
    }
});

// GET method to get the average data of temperatureC, temperatureF, and pressure
router.get('/average-gps-data', async (req, res) => {
    try {
        const avgData = await GpsData.aggregate([
            {
                $group: {
                    _id: null,  // Group by null to calculate the overall average
                    avgTemperatureC: { $avg: "$temperatureC" },
                    avgTemperatureF: { $avg: "$temperatureF" },
                    avgPressure: { $avg: "$pressure" }
                }
            }
        ]).exec();

        res.json(avgData[0]);  // Send the result as JSON
    } catch (err) {
        console.error(err);
        res.status(500).send(err);
    }
});

module.exports = router;
