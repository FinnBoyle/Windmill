require("dotenv").config();
import * as mqtt from "mqtt";
import * as readline from "readline";
import { stdin as input, stdout as output } from "node:process";
import express, { Request, Response } from "express";
import bodyParser, { json } from "body-parser";
import cors from "cors";
import Metrics from "./metrics";

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
let latestMetrics: any = {};
client.on("connect", function () {
  client.subscribe("TURBINE_FEED");
});

//Define http routes and MQTT pub/subs
app.get("/metrics", async (request: Request, response: Response) => {
  response.json(Object.values(latestMetrics));
});
app.put("/settings", async (request: Request, response: Response) => {
  client.publish(
    "SETTINGS_" + request.body.id,
    `{"stepperState":${parseInt(request.body.stepperState)},"kp": ${
      request.body.kp
    },"ki":${request.body.ki},"kd":${request.body.kd}}`
  );
  response.json(request.body);
});

app.listen(process.env.PORT);

console.log(
  `Express server has started on port ${process.env.PORT}. Open http://localhost:${process.env.PORT}/ to see results`
);
client.on("message", function (topic, message) {
  // message is Buffer
  if (topic == "TURBINE_FEED") {
    let obj = JSON.parse(message.toString());
    console.log(obj);
    let metrics = new Metrics({ ...obj });
    latestMetrics[metrics.id] = metrics;
  }
});

// Use recursion rather than infinite loop
// command line input
const rl = readline.createInterface({ input, output });
const pubInTopic = () => {
  rl.question("Type a message to publish to inTopic! ", (answer) => {
    if (answer == "exit") {
      rl.close();
    }
    //{"type":"metrics","voltage":1.65,"stepperState":3,"kp":"0","ki":"0","kd":"0"}
    client.publish(
      "SETTINGS_T1",
      `{"type":"settings","voltage":1.65,"stepperState":${parseInt(
        answer
      )},"kp":"0","ki":"0","kd":"0"}`
    );
    pubInTopic();
  });
};
pubInTopic();
