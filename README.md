# Wind Turbine Project
## Project Requirements
#### Pre-requisites
- Install [Neo4j Desktop](https://neo4j.com/download/)
- Create a project on Neo4j Desktop
- Install [Neo4j APOC Library](https://neo4j.com/developer/neo4j-apoc/)
The APOC Library must be installed within any project you wish to import or export from.
#### Exporting Requirements
- Open Neo4j Browser within the created project and type the following command. This will create a requirements.json file within your {projectDir}/import folder.
```
CALL apoc.export.json.all("requirements.json");
```
#### Importing Requirements
- Open Neo4j Browser within the created project and type the following commands. This will read the requirements file from your {projectDir}/import folder of the project you wish to import into. 
- This must be a blank project otherwise there may be import conflicts. 
- Constraints must be applied first.
```
CREATE CONSTRAINT FOR  (n:Standard) REQUIRE n.neo4jImportId IS UNIQUE;
CREATE CONSTRAINT FOR (n:Requirement) REQUIRE n.neo4jImportId IS UNIQUE;
CREATE CONSTRAINT FOR (n:Implementation) REQUIRE n.neo4jImportId IS UNIQUE;
CALL apoc.import.json("requirements.json");
```
### Required by all components
#### Pre-requisites

- Install [Node.js](https://nodejs.org/en/)
- Install [Arduino IDE](https://www.arduino.cc/en/software)

#### Getting started

- Clone the repository

```
git clone  https://github.com/emsoftaut/2022-2023-Windmill-Project.git
```
## Arduino Uno Stepper motor Controller
#### Install dependencies
The following libraries must be installed from the Arduino IDE:
- ArduinoJSON
- EspSoftwareSerial
## ESP2866 for Communications
The following libraries must be installed from the Arduino IDE:
- ArduinoJSON
- EspSoftwareSerial
- PID_v2
- PubSubClient
## ExpressJS Web Server
#### Install dependencies
```
cd turbine_server
npm install i
```
#### Environment vars

This project uses the following environment variables:
These can be defined in a .env file within the component folder.

| Name        | Description               |
| ----------- | ------------------------- |
| PORT        | RESTful api endpoint port |

### NPM Run Scripts

```
npm run {script}
```

| Npm Script         | Description                                                         |
| ------------------ | ------------------------------------------------------------------- |
| `start`            | Run the React project               |

## ReactJS Control Panel
#### Install dependencies
```
cd turbine_react
npm install i
```
### NPM Run Scripts

```
npm run {script}
```

| Npm Script         | Description                                                         |
| ------------------ | ------------------------------------------------------------------- |
| `start`            | Run the React project               |




