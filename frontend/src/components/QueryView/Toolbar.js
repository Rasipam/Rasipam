import {inject, observer} from "mobx-react";
import {useTranslation} from "react-i18next";
import {Button} from "@mui/material";
import {Check, Undo, Visibility} from "@mui/icons-material";
import strings from "../../static/strings";
import Toolbar from "../Common/Toolbar";

const QueryViewToolbar = inject('analysis')(observer(({
                                                          analysis,
                                                          queryParams,
                                                      }) => {
    const applicable = queryParams.isApplicable();
    const disabled = !analysis.stateEditable;

    const {t} = useTranslation();
    const style = {
        pt: 0.25,
        pb: 0.25,
    }
    return <Toolbar elements={[
        !applicable && <Button size={'small'}
                               disabled={analysis.history.length <= 1}
                               startIcon={<Undo/>}
                               sx={style}
                               onClick={analysis.undo}>
            {t(strings.Undo)}
        </Button>,
        applicable && !analysis.isPreviewing && <Button size={"small"}
                                                        startIcon={<Visibility/>}
                                                        disabled={disabled}
                                                        sx={style}
                                                        onClick={() => analysis.preview(queryParams.queryParams)}>
            {t(strings.PreviewChange)}
        </Button>,
        analysis.isPreviewing && <Button size={"small"}
                                         startIcon={<Check/>}
                                         disabled={disabled}
                                         sx={style}
                                         onClick={() => {
                                             analysis.applyChange();
                                             queryParams.clearQueryParams();
                                         }}>
            {t(strings.ApplyChange)}
        </Button>,
    ].filter(Boolean)}/>;
}));

export default QueryViewToolbar;
