/**
 * Created by Jiang Wu on 2022/1/27
 */

import {inject, observer} from "mobx-react";
import {Box, Stack} from "@mui/material";
import NLQuery from "./NLQuery";
import QueryHistory from "./QueryHistory";
import {useQueryParams} from "./useQueryParams";
import QueryViewToolbar from "./Toolbar";

const QueryView = inject('analysis')(observer(({
                                                   queryParams,
                                                   analysis,
                                               }) => {
    const disabled = !analysis.stateEditable;

    return <Stack width={'100%'}
                  height={'100%'}>
        <Box margin={1}
             flex={'0 0 auto'}>
            <NLQuery {...queryParams} disabled={disabled}/>
        </Box>
        <Box margin={1}
             marginTop={0}
             flex={1}
             overflow={'hidden'}>
            <QueryHistory/>
        </Box>
    </Stack>;
}));

export default function useQueryView() {
    const queryParams = useQueryParams();

    return {
        view: <QueryView queryParams={queryParams}/>,
        toolbar: <QueryViewToolbar queryParams={queryParams}/>,
    }
}
