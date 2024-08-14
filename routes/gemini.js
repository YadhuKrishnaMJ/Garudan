const express = require('express');       // Express Module for creating express app
const router = express.Router();          // Express Router for routing
const GpsData = require('../models/GpsData'); // Gps data model in MongoDB
require('dotenv').config();
const { GoogleGenerativeAI } = require('@google/generative-ai');
const genAI = new GoogleGenerativeAI(process.env.GEMINI_KEY);

router.get('/average-gemini', async (req, res) => {
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
        const { avgTemperatureC, avgTemperatureF, avgPressure } = avgData[0];

        let text = ""; // Use 'let' to allow reassignment

        async function run() {
            const model = genAI.getGenerativeModel({ model: "gemini-pro" });

            const prompt = `Write a neat report for the following temperature and pressure of the IOT system that 
            monitors the product health in supply chain system. The Average temperature in celcius is ${avgTemperatureC},The Average Temperature in 
            Farenheit is ${avgTemperatureF} and finally the average pressure is ${avgPressure}. Write a neat report`;

            const result = await model.generateContent(prompt);
            const response = await result.response;

            text = response.text(); // Assign the generated text to the 'text' variable
        }

        await run(); // Await the execution of 'run' to ensure 'text' is set

        res.json({ 
            averageData: avgData,
            generatedText: text 
        });

    } catch (err) {
        console.error(err);
        res.status(500).send(err);
    }
});

module.exports = router;
