function toggleMenu() {
    var menu = document.getElementById('menu');
    if (menu.style.display === 'block') {
        menu.style.display = 'none';
    } else {
        menu.style.display = 'block';
    }
}
function toggleChatbot() {
    const chatbotBox = document.getElementById('chatbot-box');
    chatbotBox.classList.toggle('show');
}

//Function to fetch report from gemini
async function fetchReport() {
    try {
        const response = await fetch('gemini-api/average-gemini');
        const data = await response.json();

        const reportContainer = document.getElementById('chatbot-content');

        if (data.generatedText) {
            reportContainer.innerHTML = `
                <h2>Average Data</h2>
                <p><strong>Temperature (Celsius):</strong> ${data.averageData[0].avgTemperatureC}</p>
                <p><strong>Temperature (Fahrenheit):</strong> ${data.averageData[0].avgTemperatureF}</p>
                <p><strong>Pressure:</strong> ${data.averageData[0].avgPressure}</p>
                <h2>Generated Report</h2>
                <p><strong><u>IOT Health Monitoring Report</u></strong></p>
                <p><strong>Temperature:</strong></p>
                <ul>
                    <li>Average Temperature (Celsius): ${data.averageData[0].avgTemperatureC}</li>
                    <li>Average Temperature (Fahrenheit): ${data.averageData[0].avgTemperatureF}</li>
                </ul>
                <p><strong>Pressure:</strong></p>
                <ul>
                    <li>Average Pressure: ${data.averageData[0].avgPressure}</li>
                </ul>
                <p><strong>Summary:</strong> The IOT system monitoring the product health in the supply chain system indicates an average temperature of ${data.averageData[0].avgTemperatureC} degrees Celsius or ${data.averageData[0].avgTemperatureF} degrees Fahrenheit. The average pressure is ${data.averageData[0].avgPressure}. These values are within the acceptable range for maintaining product integrity. The system is functioning effectively in monitoring the product health.</p>
            `;
        } else {
            reportContainer.innerHTML = '<p>No report generated.</p>';
        }
    } catch (error) {
        console.error('Error fetching the report:', error);
        document.getElementById('report-container').innerHTML = '<p>Failed to load report.</p>';
    }
}
