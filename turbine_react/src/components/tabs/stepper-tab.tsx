import * as React from "react";
import Radio from "@mui/material/Radio";
import RadioGroup from "@mui/material/RadioGroup";
import FormControlLabel from "@mui/material/FormControlLabel";
import FormControl from "@mui/material/FormControl";
import FormLabel from "@mui/material/FormLabel";
import Grid from "@mui/material/Grid";
import Button from "@mui/material/Button";

import TextField from "@mui/material/TextField";
import FormGroup from "@mui/material/FormGroup";
type StepperTabProps = {
  onFormSubmit: (e: React.FormEvent<HTMLFormElement>) => void;
  onStateChange: (e: React.ChangeEvent<HTMLInputElement>) => void;
};
const StepperTab: React.FC<StepperTabProps> = (props: StepperTabProps) => {
  const { onFormSubmit, onStateChange } = props;
  return (
    <form>
      <Grid>
        <Grid item xs={12}>
          <FormControl>
            <FormLabel>Orientation Stepper State</FormLabel>
            <RadioGroup
              row
              defaultValue={0}
              id="stepperState"
              onChange={(e: React.ChangeEvent<HTMLInputElement>) =>
                onStateChange(e)
              }
            >
              <FormControlLabel
                value={0}
                control={<Radio />}
                label="OFF"
                id="stateOff"
              />
              <FormControlLabel
                value={1}
                control={<Radio />}
                label="CLOCKWISE_AUTO"
                id="stateClockwise"
              />
              <FormControlLabel
                value={2}
                control={<Radio />}
                label="ANTI_CLOCKWISE_AUTO"
                id="stateAntiClockwise"
              />
              <FormControlLabel
                value={3}
                control={<Radio />}
                label="PID_FAKE"
                id="statePIDFAKE"
              />
              <FormControlLabel
                value={4}
                control={<Radio />}
                label="PID"
                id="statePID"
              />
              <FormControlLabel
                value={5}
                control={<Radio />}
                label="CLOCKWISE_STEP"
                id="stateClockwise"
              />
              <FormControlLabel
                value={6}
                control={<Radio />}
                label="ANTI_CLOCKWISE_STEP"
                id="stateAntiClockwise"
              />
            </RadioGroup>
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
