Welcome to Garudan!

Instructions for Local Demo:

1) Enter the following command after clone to download all Dependencies

npm install

2) Install localtunnel package globally (it is recommended to install the package globally)

npm install -g localtunnel

3) Edit the .env file with PORT number of your choice and "Your" mongodb atlas connection string(node.js)

4) Run local server

npm start

5) expose local server using the following in different terminal and notedown the domain link (change port no. accordingly)

lt --port 3000 


6) Flash your esp32 Module with code example given in "ESP32 code" folder, change pin values depending on your connections. Also change the local tunnel domain link in HTTP post function.

7) Finally enable ESP32 module to send data to server