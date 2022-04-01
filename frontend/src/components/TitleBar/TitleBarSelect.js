import Select from "../Common/Select";
import {alpha} from "@mui/material";
import {useTheme} from "@mui/styles";

export default function TitleBarSelect(props) {
    const theme = useTheme();
    const color = theme.palette.primary.contrastText;
    const fadeColor = alpha(theme.palette.primary.contrastText, 0.3);
    const padding = theme.spacing(0, 3, 0, 2);
    return <Select {...props}
                   sx={{
                       label: {
                           color,
                       },
                       select: {
                           '& .MuiInput-input': {
                               color,
                               padding,
                           },
                           '& .MuiSelect-icon': {
                               color,
                           },
                           '& .MuiInput-root:before': {
                               borderBottom: `1px solid ${fadeColor}`,
                           },
                           '& .MuiInput-root:hover:not(.Mui-disabled):before': {
                               borderBottom: `2px solid ${fadeColor}`,
                           },
                           '& .MuiInput-root:after': {
                               borderBottom: `2px solid ${color}`,
                           },
                           color,
                       },
                   }}/>
}