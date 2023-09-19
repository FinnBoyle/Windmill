import * as React from "react";
import FormControl from "@mui/material/FormControl";
import FormLabel from "@mui/material/FormLabel";
import Grid from "@mui/material/Grid";
import Button from "@mui/material/Button";

import TextField from "@mui/material/TextField";
import FormGroup from "@mui/material/FormGroup";
import { MenuItem } from "@mui/material";
type StepperTabProps = {
  onFormSubmit: (e: React.FormEvent<HTMLFormElement>) => void;
  onStateChange: (e: React.ChangeEvent<HTMLInputElement>) => void;
  stepperState: number;
};

enum StepperState {
  OFF,
  CLOCKWISE_AUTO,
  ANTI_CLOCKWISE_AUTO,
  PID_FAKE,
  PID,
  CLOCKWISE_STEP,
  ANTI_CLOCKWISE_STEP,
  MOVE_STEPS_AND_BACK, //used to implement chatgpt code
  //testing auto functions below, chatgpt or otherwise
  AUTO_NO_HELP,
  AUTO_FOUNDATIONS,
  AUTO_FINISHINGS,
  AUTO_FULL,
  AUTO_CONCERT,
  //Testing the avoid function below
  SEEK_NO_HELP,
  SEEK_FOUNDATIONS,
  SEEK_FINISHINGS,
  SEEK_FULL,
  SEEK_CONCERT
}

const StepperTab: React.FC<StepperTabProps> = (props: StepperTabProps) => {
  const { onFormSubmit, onStateChange, stepperState } = props;
  const states = Object.keys(StepperState).filter((v) => isNaN(Number(v)));
  const stateNumbers = Object.values(StepperState).filter(
    (v) => !isNaN(Number(v))
  );
  return (
    <form onSubmit={(event) => onFormSubmit(event)}>
      <Grid>
        <Grid item xs={12}>
          <FormControl>
            <FormLabel>Stepper Orientation State</FormLabel>
            <TextField
              id="stepperState"
              select
              value={stepperState}
              onChange={onStateChange}
              sx={{ mx: 1 }}
            >
              {states.map((state, i) => (
                <MenuItem key={stateNumbers[i]} value={stateNumbers[i]}>
                  {state}
                </MenuItem>
              ))}
            </TextField>
          </FormControl>
        </Grid>
        <Grid item xs={12}>
          <FormControl>
            <FormLabel>Stepper Parameters</FormLabel>
            <FormGroup row sx={{ justifyContent: "space-around" }}>
              <TextField
                id="rpm"
                label="RPM"
                variant="outlined"
                type="number"
                sx={{ mx: 1 }}
              />
              <TextField
                id="steps"
                label="Steps"
                variant="outlined"
                type="number"
                sx={{ mx: 1 }}
              />
            </FormGroup>
          </FormControl>
        </Grid>
      </Grid>
      <Button type="submit" variant="contained" sx={{ mt: 2 }}>
        Submit
      </Button>
    </form>
  );
};

export default StepperTab;
