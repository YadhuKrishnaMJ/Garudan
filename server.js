//Required modules are imported

require('dotenv').config();     //Dotenv for environment variables
const express = require('express');     //Express app
const bodyParser = require('body-parser');    //BodyParser middleware, Parse json objects etc
const mongoose = require('mongoose');     //For Local MongoDB connection
const gpsDataRoutes = require('./routes/gpsData');     //Contains all routes for gps api

const app = express();      //Create an Express App
const port = process.env.PORT || 3000;    //Access environment variable port

app.use(bodyParser.json());     //Parse incoming requests with json payload
app.use(express.static('public'));    //Serve static pages in public


//Connect to the MongoDB Connection
try{
  mongoose.connect(process.env.MONGODB_URI, { useNewUrlParser: true, useUnifiedTopology: true });   //Connect with mongoDB, Options for new MongoDB Driver
  console.log('Connected to GarudanDB successfully!');
}
catch(err){
  console.log(`Error connecting to the database ${err}`);
}


app.use('/api', gpsDataRoutes);   //Use the gps api routes


//Listen in Port
app.listen(port, () => {   
  console.log(`Server running at http://localhost:${port}/`);
});
