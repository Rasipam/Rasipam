/**
 * Created by Jiang Wu on 2022/1/29
 */

import {inject, observer} from "mobx-react";
import {Box, Button, Chip, Divider, IconButton, Menu, MenuItem, TextField} from "@mui/material";
import {Add} from "@mui/icons-material";
import {useTranslation} from "react-i18next";
import strings from "../../static/strings";
import {useState} from "react";
import {findQueryType, types} from "./useQueryParams";
import {scrollbarSize} from "../../static/theme";
import {useTheme} from "@mui/styles";

function TypesMenu({anchorEl, onClose, onSelect}) {
    const typeItems = [];
    types.forEach(tc => {
        typeItems.push(
            <Divider key={tc.objective}>{tc.objective}</Divider>
        )
        tc.types.forEach(t => {
            typeItems.push(
                <MenuItem key={t.type}
                          onClick={() => onSelect(t.type)}>
                    {t.type}
                </MenuItem>
            )
        })
    });
    return <Menu anchorEl={anchorEl}
                 open={Boolean(anchorEl)}
                 onClose={onClose}>
        {typeItems}
    </Menu>;
}

function ParamsMenu({anchorEl, onClose, onSelect, type}) {
    const t = findQueryType(type);
    return <Menu anchorEl={anchorEl}
                 open={Boolean(anchorEl)}
                 onClose={onClose}>
        {t && t.params.map(p => (
            <MenuItem key={p}
                      onClick={() => onSelect(p)}>{p}</MenuItem>
        ))}
    </Menu>;
}

function FormQuery({queryParams, setQueryParams, onStart, onEnd, disabled}) {
    const {t} = useTranslation();
    const theme = useTheme();

    const [typeBtn, setTypeBtn] = useState(null);
    const handleOpenTypesMenu = e => {
        onStart();
        setTypeBtn(e.currentTarget)
    }
    const handleCloseTypesMenu = () => {
        onEnd();
        setTypeBtn(null);
    }
    const handleSelectType = t => {
        setQueryParams({
            type: t,
            params: {},
        });
        handleCloseTypesMenu();
    }

    const [paramBtn, setParamBtn] = useState(null);
    const handleOpenParamMenu = e => setParamBtn(e.currentTarget)
    const handleCloseParamMenu = () => setParamBtn(null);
    const [inputParam, setInputParam] = useState(null);
    const handleSelectParam = param => {
        setInputParam([param, '']);
        handleCloseParamMenu();
    };
    const handleChangeInputParam = e => setInputParam([inputParam[0], e.target.value]);
    const handleInputParam = () => {
        setQueryParams({
            params: {
                ...queryParams.params,
                [inputParam[0]]: inputParam[1],
            }
        });
        setInputParam(null);
    }
    const handleDelete = key => () => {
        const params = JSON.parse(JSON.stringify(queryParams.params));
        delete params[key];
        setQueryParams({params});
    };

    return <Box display={'flex'} height={34}>
        <Button sx={{flex: '0 0 auto'}} size={"small"} onClick={handleOpenTypesMenu} disabled={disabled}>
            {t(queryParams.type || strings.EmptyType)}
        </Button>
        {
            queryParams.type && !inputParam &&
            <IconButton sx={{flex: '0 0 auto'}} size={"small"} onClick={handleOpenParamMenu} disabled={disabled}>
                <Add/>
            </IconButton>
        }
        {
            Boolean(inputParam) &&
            <TextField value={inputParam[1]}
                       label={inputParam[0]}
                       variant={"filled"}
                       size={"small"}
                       autoFocus
                       onChange={handleChangeInputParam}
                       disabled={disabled}
                       sx={{
                           flex: '0 0 100px',
                           height: 40,
                           '& .MuiInputBase-input': {pt: '10px', pb: '1px', pl: 1, pr: 1},
                           '& .MuiInputLabel-root': {transform: `translate(${theme.spacing(1)}, 6px) scale(1)`},
                           '& .MuiInputLabel-shrink': {transform: `translate(${theme.spacing(1)}, 0px) scale(0.6)`},
                       }}
                       onKeyDown={e => e.key === 'Enter' && handleInputParam()}/>
        }
        <Box sx={{flex: '1 0 0%', transform: `translateY(${scrollbarSize}px)`}}
             overflow={'scroll hidden'}
             display={'flex'}
             flexWrap={'nowrap'}
             alignItems={'center'}>
            {
                queryParams.params &&
                Object.entries(queryParams.params)
                    .filter(([key, value]) => value !== undefined)
                    .map(([key, value]) => (
                        <Chip key={key}
                              size={"small"}
                              label={`${key}: ${value}`}
                              sx={{marginLeft: 0.5}}
                              disabled={disabled}
                              onDelete={handleDelete(key)}/>
                    ))
            }
        </Box>

        <TypesMenu anchorEl={typeBtn}
                   onClose={handleCloseTypesMenu}
                   onSelect={handleSelectType}/>

        <ParamsMenu anchorEl={paramBtn}
                    type={queryParams.type}
                    onClose={handleCloseParamMenu}
                    onSelect={handleSelectParam}/>
    </Box>;
}

export default inject()(observer(FormQuery));
