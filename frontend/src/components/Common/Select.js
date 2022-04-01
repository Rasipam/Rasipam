/**
 * Created by Jiang Wu on 2022/1/25
 */

import React from 'react';
import {Box, Checkbox, ListItemText, MenuItem, TextField, Typography} from "@mui/material";
import {useTheme} from "@mui/styles";

function Select({label, options, value, onChange, sx, multiple}) {
    const theme = useTheme();
    return <Box sx={{display: 'flex', alignItems: 'flex-end'}}>
        <Typography sx={sx && sx.label}>
            {label}:
        </Typography>
        <div sx={{marginRight: theme.spacing(1)}}/>
        <TextField select
                   SelectProps={{
                       multiple,
                       renderValue: multiple ?
                           (selected) => selected.join(', ') :
                           undefined,
                   }}
                   size={"small"}
                   sx={sx && sx.select}
                   value={value}
                   onChange={onChange}
                   variant={'standard'}>
            {options.map(item => {
                const itemVal = item.value || item;
                const itemLab = item.label || item.value || item;
                return <MenuItem key={itemVal}
                                 value={itemVal}>
                    {multiple ?
                        <React.Fragment>
                            <Checkbox checked={value.includes(itemVal)}/>
                            <ListItemText primary={itemLab}/>
                        </React.Fragment> :
                        itemLab}
                </MenuItem>
            })}
        </TextField>
    </Box>;
}

export default React.memo(Select);
