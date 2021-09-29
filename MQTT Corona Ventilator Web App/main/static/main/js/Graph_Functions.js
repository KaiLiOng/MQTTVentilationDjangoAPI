/* Title: Timebased Callback Functions &  basic plots (Line, Piechart)
*
* Version: v1.0
* Autor: Jeremy Kunz Heiko Maier-Gerber
* Date: 30.01.2020
*
* Short Description:
* Simple timebased callback function, and plotting of two basic plots: pie chart and line chart
*
* Output Data:
* Recieved Values via ESP-NOW and Sensor/Actor Values, send to the Server
* Visualization of In/Exhaling via WS2812b LED Strip
* 
* Devices to connect:
* 2 ESPs, that run the codes for SensorActor and Communication ESP 
* 
*
* Changelog:
* 22.12.2020 First Version - Heiko-Maier-Gerber
* 30.10.2020 Added Timebased Callback function - Jeremy Kunz
*/

var redColor = 'rgb(181, 0, 6)';
var blueColor = 'rgb(0, 51, 192)';
var greenColor = 'rgb(13, 175, 40)';
var client;
var reconnectTimeout = 2000;
var host = "broker.hivemq.com";
//var host = "broker.emqx.io";
var port = 8000;


MQTTConnect();

function MQTTConnect() {
  client = new Paho.MQTT.Client(host, port, "clientId");
  client.onConnectionLost = onConnectionLost;
  client.onMessageArrived = onMessageArrived;

  //client.disconnect();

  client.connect({
      timeout: 360000,
      onSuccess: onConnect,
      onFailure: onFailure
  });
  console.log("MQTTConnect Thread");
}

function onConnect() {
  console.log("Connected");
  client.subscribe("CoronaVentilator/State");

  var ESPmessage = new Paho.MQTT.Message("I'm in");
  console.log(ESPmessage);
  ESPmessage.destinationName = "CoronaVentilator/Command";
  ESPmessage.qos = 2;
  client.send(ESPmessage);
}


function onFailure() {
  //alert("8x8-WebApp MQTT connection is unable to connect to " + MQTTBroker + " at port " + MQTTPort + "\nPlease check the MQTT broker and MQTT port!");
  //localStorage.setItem("mqttestablished", false);
  console.log("onFailure:" + responseObject.errorMessage);
  //console.log("mqtt status: reconnect");
    MQTTConnect();
}

function onConnectionLost(responseObject) {
  if (responseObject.errorCode !== 0) {
      //localStorage.setItem("mqttestablished", false);
      console.log("onConnectionLost: " + responseObject.errorMessage);
      //alert("Error " + responseObject.errorCode + " ( " + responseObject.errorMessage + " ) " + ": Please disconnect before attempting connection to the MQTT broker next time!");
  }
    MQTTConnect();
};

var StateSetAdr;
var StateSetValue;
var ValueArr = [];
var DataTimeArr = [];
var ValueCountArr = [];

function onMessageArrived(message) {
  //console.log("onMessageArrived: " + message.payloadString + " " + message.destinationName);

  var dateTime = new Date().toLocaleString();
  var status = message.payloadString.toString();
  var statustrim = status.replace("[","").replace("]","");
  //console.log(statustrim);
  var StatusObj = JSON.parse(statustrim);
  //console.log("StatusObj.ADR: " + StatusObj.ADR + ", StatusObj.BAT: " + StatusObj.BAT);
  StateSetAdr = StatusObj.ADR;
  StateSetValue = StatusObj.BAT;
  ValueArr.push(StateSetValue);
  DataTimeArr.push(dateTime);
  ValueCountArr.push(dateTime);
  //console.log(ValueArr);
  //console.log(DataTimeArr);
  //mqttconnected = StatusObj.ADR;
  plotLineChart();
};


function plotLineChart() {
  var linechart = document.getElementById('myChart').getContext('2d');
  //console.log("Chart Data : " + ValueArr);
  //console.log("Chart Time : " + DataTimeArr);
  
  var mylinechart = new Chart(linechart, {
      type: 'line',
      data: {
          labels: DataTimeArr/*@Html.Raw(Model.ChartTimestamp)*/,
          datasets: [{
              label: 'Data',
              data: ValueArr,/*@Html.Raw(Model.ChartData),*/
              fill: false,
              borderColor: 'rgb(75, 92, 192)',
           }]
       },
       option:{
         responsive: true,
         animation:{
           duration: 0
         },
         plugins:{
           legend:{
             position:'top',
           },
           title:{
             display:true,
           }
         }
       }
      //  options:{
      //    scales:{
      //      x:{
      //        type:'realtime',
      //        realtime:{
      //          duration: 20000,
      //          refresh: 1000,
      //          delay: 2000,
      //        }
      //      },
      //      y:{
      //        title:{
      //          display:false,
      //          text:'Value'
      //        }
      //      },
      //      ticks:{
      //        display: false
      //      }
      //    },
      //    interaction:{
      //      intersect:false
      //    }
      //  }
  });

  // chart.update(none);

  if (ValueArr.length>1800){
    ValueArr.splice(0,1);
    DataTimeArr.splice(0,1);
  }



//   var linechart2 = document.getElementById('graph').getContext('2d');
//   var data = [{
//     x:[DataTimeArr],
//     y:[ValueArr],
//     mode: 'lines',
//     line:{color: '#80CAF6'}
//   }];

//   Plotly.newPlot(linechart2,data);

//   var cnt = 0;

//   var interval = setInterval(function() {  
//   var update = {
//   x:  [[DataTimeArr]],
//   y: [[ValueArr]]
//   }
  
//   Plotly.extendTraces(linechart2, update, [0])
  
//   if(cnt === 100) clearInterval(interval);
// }, 1000);

  // handleData(StateSetValue, "line", 1, "OutFlow",  xAxis_1, "InFlow", compareValue );
      //window.location = window.location;

  }


// function getData2(){
//   return Math.random();
// }

// Plotly.newPlot('graph',[{
//   y:[getData2()],
//   type:'line'
// }]);

//Handel Data function -- checks if call was made by line or pie chart
function handleData(data, chartType, resfresh_rate, sensortype,  xAxis_1, chartTitle, compareValue )
{
  switch(chartType)
  {
    case "line":
      redrawLineChart(data, resfresh_rate, sensortype, chartType);
      break;
    case "pie":
      redrawPieChart(data, sensortype, chartType);
      break;
    case "text":
      redrawText(data, sensortype, chartType);
      break;
    case "bar":
      redrawBar(data, chartType, sensortype , xAxis_1, chartTitle, compareValue );
        break;
    default:
      console.log("Unknown type");
      break;
  }
}


function redrawBar(data, chartType, sensortype, xAxis_1, chartTitle, compareValue)
{

  var graph_dom = sensortype + "_" + chartType;
  console.log(xAxis_1)
  var trace1 = {
    x: [xAxis_1],
    y: [data],
    name: chartTitle,
    type: 'bar'
  };
  
  var trace2 = {
    x: [xAxis_1],
    y: [compareValue],
    name: 'Standardwert',
    type: 'bar'
  };
  
  var data = [trace1, trace2];
  
  var layout = {barmode: 'group'};
  
  Plotly.newPlot(graph_dom, data, layout);

}

var xcounter =0;        // counts x values

function redrawText(dataR, graph_ID, sensortype)
{

  var graph_dom = graph_ID + "_" + sensortype;

  var data = [
    {
      type: "indicator",
      mode: "number+delta",
      value: dataR,

      number: { suffix: "%"},

      delta: { position: "bottom", reference: "200" },
      domain: { x: [0, 1], y: [0, 1] }
    }
  ];
  
  var layout = {
    margin: { t: 0, b: 0, l: 0, r: 0 },
    template:{
      data:{
        indicator: [
          {
            title: {text: "TEXT"},
          }
        ]
      }
    }
  };

  var config = {responsive: true}
  Plotly.newPlot(graph_dom, data, layout, config);
}

//Adding new Data point to line chart
function redrawLineChart(data, resfresh_rate, graph_ID, sensortype){

  var graph_dom = graph_ID + "_" + sensortype;
  xcounter = xcounter + (resfresh_rate/1000);
  Plotly.extendTraces(graph_dom,{
    x: [[xcounter]],
    y:[[data]]
    }, [0]);    
  if(xcounter > 1000) {                                                                          
      Plotly.relayout(graph_dom,{        
        xaxis: {
          range: [xcounter-1000,xcounter]                                                                             
        }
      });
  }
}

// "Add" new Datapoint to PieChart
function redrawPieChart(dataR, sensortype, chartType )
{

  var graph_dom = sensortype + "_" + chartType;


  var first_value= dataR;
  var second_value = 100-first_value;
  var data = [{
    values: [first_value, second_value],
    labels: ["Oxygen", "Air"],
    type: 'pie'
  }];
  var config = {responsive: false}

  Plotly.newPlot(graph_dom, data, config);     //Plot new Pie chart --> slower than relayout, but only works for line chart
}



//Get data from Django REST API with AJAX (Asynchon Javascript and XML )
function getData(patient,sensortype, chartType, xAxis_1, chartTitle, compareValue )                                                                  // Later this function will receive data from SQL
{
  //AJAX call 
  $.ajax({
    url: '/api/detailValueInfo/' + patient + '/' + sensortype,
    type: 'get',
    contentType: 'application/json',
    //Time based callback function
    success: function(data) 
      {
        var json = JSON.parse(data);
        setTimeout(function(){handleData(json[0].value, chartType, 1, sensortype,  xAxis_1, chartTitle, compareValue );},1);
        setTimeout(function(){getData(patient,sensortype, chartType, xAxis_1, chartTitle, compareValue);}, 1)
      }
  });
}

//####################################################### 
//#################### Line Diagram #####################

function print_Line_Diagram(chartType,patient,sensortype)                                    // function will print a Line Diagram // sensortype decides which values will be received from SQL
{

  var graph_dom = sensortype + '_' + chartType;
  switch(sensortype)
  {
    case "Pressure":
      diagramm_title = "Air Pressure Line Diagram";
      yaxis_name = "Pressure in mBar";
      set_color = blueColor;
      break;
    
    case "InFlow":
      diagramm_title = "Inhalation Airflow Diagram";
      yaxis_name = "Inhalating ml per second";
      set_color = greenColor;
      break;

    case "OutFlow":
      diagramm_title = "Exhalation Airflow Diagram";
      yaxis_name = "Exhalating ml per second";
      set_color = redColor;
      break;
        
    default:
      diagramm_title = "Sensor Type unknown";
      yaxis_name = "Unknow";
      set_color = redColor;
      break;
  }
    var layout = {                                                                                                      // defines Layout
        title: diagramm_title,
        xaxis: {title: 'Time in seconds'},                                                                              // xaxis name
        yaxis: {title: yaxis_name},
      };

    var data_layout = [{                                                                                              // contains values to print
      x: [0],                                                                                                           // x axis start value 0sec
      y:[getData(patient, sensortype, chartType)],                                                                                                    // call Function to get data from SQL
      mode:'lines+markers',                                                                                             // plot type
      name: yaxis_name,
      line: {shape: 'spline',
             color: set_color,
            },                                                                                          
    }];
    var config = {responsive: true}

    Plotly.plot(graph_dom,data_layout,layout, config);                                                                         // create Plot //parameters are: 

}


//####################################################### 
//#################### Digital Value ####################
function print_digital(chartType, patient, sensortype, chartTitle)
{
  
  var graph_dom = sensortype + "_" + chartType;

  var data = [
    {
      type: "indicator",
      mode: "number+delta",
      value: getData(patient,sensortype, chartType),

      number: { suffix: "%"},

      delta: { position: "bottom", reference: "200" },
      domain: { x: [0, 1], y: [0, 1] }
    }
  ];
  
  var layout = {

    margin: { t: 0, b: 0, l: 0, r: 0 },
    template:{
      data:{
        indicator: [
          {
            title: {text: chartTitle},
          }
        ]
      }
    }
  };

  var config = {responsive: true}
  Plotly.newPlot(graph_dom, data, layout, config);
}


//####################################################### 
//###################### Pie Chart ######################

function print_pie_chart(chartType, patient, sensortype ,graphTitle)
{

  var graph_dom  = sensortype + "_" + chartType;
  var firstComponentName, lastComponentName;
  if(sensortype.includes("Composition"))
  {
    firstComponentName = "O2";
    lastComponentName = "CO2";

  }

  var first_value= getData(patient,sensortype, chartType);     //Call function to get new Data
  var second_value = 100-first_value;
  var data = [{
    values: [first_value, second_value],
    labels: [firstComponentName, lastComponentName],
    type: 'pie'
  }];

  var layout = {
    title: graphTitle
  };
  var config = {responsive: true}

  Plotly.newPlot(graph_dom, data, layout, config);
}

function print_bar_chart(chartType, patient, sensortype, xAxis_1, chartTitle, compareValue)
{


  var graph_dom = sensortype + "_" + chartType;
  var trace1 = {
    x: [xAxis_1],
    y: [getData(patient,sensortype, chartType, xAxis_1, chartTitle, compareValue)],
    name: chartTitle,
    type: 'bar'
  };
  
  var trace2 = {
    x: [xAxis_1],
    y: [compareValue],
    name: 'Standardwert',
    type: 'bar'
  };
  
  var data = [trace1, trace2];
  
  var layout = {barmode: 'group'};
  
  Plotly.newPlot(graph_dom, data, layout);

}