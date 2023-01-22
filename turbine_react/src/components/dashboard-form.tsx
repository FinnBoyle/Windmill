import * as React from "react";
import Radio from "@mui/material/Radio";
import RadioGroup from "@mui/material/RadioGroup";
import FormControlLabel from "@mui/material/FormControlLabel";
import FormControl from "@mui/material/FormControl";
import FormLabel from "@mui/material/FormLabel";
import Box from "@mui/material/Box";
import Grid from "@mui/material/Grid";
import Button from "@mui/material/Button";

import TextField from "@mui/material/TextField";
import FormGroup from "@mui/material/FormGroup";

import Select, { SelectChangeEvent } from "@mui/material/Select";
import { getTurbines, Turbine } from "../models/turbine-model";
import { InputLabel, MenuItem } from "@mui/material";

type DashboardFormProps = {
  onSubmit: (e: React.FormEvent<HTMLFormElement>) => void;
  onChange: (e: React.ChangeEvent<HTMLInputElement>, target: string) => void;
  turbines: Turbine[];
  selectedId: string;
};
const DashboardForm: React.FC<DashboardFormProps> = (
  props: DashboardFormProps
) => {
  const { turbines, selectedId, onSubmit, onChange } = props;
  return (
    <Box sx={{ mt: 1 }}>
      <form onSubmit={(event) => onSubmit(event)}>
        <Grid>
          <Grid item xs={12}>
            <FormControl>
              <FormLabel>Select a turbine</FormLabel>
              <TextField
                id="id"
                label="ID"
                select
                value={selectedId}
                onChange={(e: React.ChangeEvent<HTMLInputElement>) =>
                  onChange(e, "ID")
                }
              >
                {turbines.map((turbine) => (
                  <MenuItem key={turbine.id} value={turbine.id}>
                    {turbine.id}
                  </MenuItem>
                ))}
              </TextField>
            </FormControl>
          </Grid>
          
          
          <Grid item xs={12} sx={{ mt: 1, mx: 1 }}>
            <Button type="submit" variant="contained">
              Submit
            </Button>
          </Grid>
        </Grid>
      </form>
    </Box>
  );
};

export default DashboardForm;
