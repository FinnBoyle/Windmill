import { ThemeProvider } from "@mui/material/styles";
import WtTheme from "./theme";
import DashboardController from "./controllers/dashboard-controller";
import CssBaseline from "@mui/material/CssBaseline";

function App() {
  return (
    <ThemeProvider theme={WtTheme()}>
      <CssBaseline />
      <DashboardController />
    </ThemeProvider>
  );
}

export default App;
