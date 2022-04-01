/**
 * Created by Jiang Wu on 2022/1/29
 */

import {inject, observer} from "mobx-react";
import HistoryItem from "./HistoryItem";
import {Box} from "@mui/material";

function QueryHistory({analysis}) {
    return <Box sx={{
        width: '100%',
        height: '100%',
        overflow: 'hidden scroll',
    }}>
        {analysis.history
            .map(h => ({
                query: h.query,
                desc_len: h.desc_len,
                lastUpdate: h.lastUpdate,
                tac_num: h.tactics.length,
            }))
            .map((query, qId, arr) => (
                <HistoryItem key={qId}
                             query={query.query}
                             time={query.lastUpdate}
                             dl={query.desc_len}
                             dlOffset={(qId === 0) ? 0 : (query.desc_len - arr[qId - 1].desc_len)}
                             tc={query.tac_num}
                             tcOffset={(qId === 0) ? 0 : (query.tac_num - arr[qId - 1].tac_num)}
                             idx={qId}/>
            ))
            .reverse()}
    </Box>;
}

export default inject('analysis')(observer(QueryHistory));
