require("dotenv").config();
import * as mqtt from "mqtt";
import * as readline from "readline";
import { stdin as input, stdout as output } from "node:process";
import express, { Request, Response } from "express";
import bodyParser, { json } from "body-parser";
import cors from "cors";
import { Metrics, PIDData, Settings } from "./model";

//init
const app = express();
app.use(bodyParser.json());
app.use(
  bodyParser.urlencoded({
    extended: true,
  })
);
app.use(cors());

const client = mqtt.connect("mqtt://localhost");
let latestMetrics: Record<string, Metrics> = {};
let latestPIDData: Record<string, PIDData> = {};
client.on("connect", function () {
  client.subscribe("TURBINE_FEED");
  client.subscribe("PID_FEED");
});

//Define http routes
app.get("/metrics", async (request: Request, response: Response) => {
  response.json(Object.values(latestMetrics));
});
app.get("/pid", async (request: Request, response: Response) => {
  response.json(Object.values(latestPIDData));
});
app.put("/settings", async (request: Request, response: Response) => {
  const settings: Settings = request.body;
  client.publish("SETTINGS_" + request.body.id, JSON.stringify(settings));
  console.log(JSON.stringify(settings));
  response.json(request.body);
});

app.listen(process.env.PORT);

console.log(
  `Express server has started on port ${process.env.PORT}. Open http://localhost:${process.env.PORT}/ to see results`
);

//Subscribe to metrics feed of all turbines
client.on("message", function (topic, message) {
  // message is Buffer
  if (topic == "TURBINE_FEED") {
    let metrics: Metrics = JSON.parse(message.toString());
    latestMetrics[metrics.id] = metrics;
  } else if (topic == "PID_FEED") {
    let pidData: PIDData = JSON.parse(message.toString());
    console.log(message.toString());
    latestPIDData[pidData.id] = pidData;
  }
});
