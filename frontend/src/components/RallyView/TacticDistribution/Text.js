import {Typography} from "@mui/material";
import {memo} from "react";

export default memo(function Text(props) {
    return <Typography component={'text'} {...props}/>
})
