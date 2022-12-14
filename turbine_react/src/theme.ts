import { createTheme } from "@mui/material";
import useMediaQuery from "@mui/material/useMediaQuery";

import * as React from "react";

const WtTheme = () => {
  const prefersDarkMode = useMediaQuery("(prefers-color-scheme: dark)");
  const theme = React.useMemo(
    () =>
      createTheme({
        palette: {
          mode: prefersDarkMode ? "dark" : "light",
        },
      }),
    [prefersDarkMode]
  );
  return theme;
};

export default WtTheme;
