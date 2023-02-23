require("dotenv").config();
import * as mqtt from "mqtt";
import * as readline from "readline";
import { stdin as input, stdout as output } from "node:process";
import express, { Request, Response } from "express";
import bodyParser, { json } from "body-parser";
import cors from "cors";
import { Metrics, PIDData, Settings } from "./model";
import { Socket } from "socket.io/dist/socket";
const { Server } = require("socket.io");
const http = require("http");

//init
const server = http.createServer(express);
const io = new Server(server);
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
    io.emit("pidFeed", pidData);
    console.log(message.toString());

    if (latestPIDData[pidData.id] == undefined) {
      latestPIDData[pidData.id] = pidData;
    } else if (latestPIDData[pidData.id].interval === pidData.interval) {
      if (
        !latestPIDData[pidData.id].errors ||
        !latestPIDData[pidData.id].rotations
      ) {
        return;
      }
      latestPIDData[pidData.id].errors.push(...pidData.errors);
      latestPIDData[pidData.id].rotations.push(...pidData.rotations);
    } else {
      latestPIDData[pidData.id] = pidData;
    }
  }
});

//Socket.io API
io.on("connection", (socket: any) => {
  console.log("New terminal connection: ", socket.id);
});

app.listen(process.env.PORT);
server.listen(5001);
