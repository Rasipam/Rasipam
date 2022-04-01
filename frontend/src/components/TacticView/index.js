/**
 * Created by Jiang Wu on 2022/1/27
 */

import {inject, observer} from "mobx-react";
import {Table, TableBody, TableContainer, TableHead, TableRow} from "@mui/material";
import {Tactic} from "./Tactic";
import {SortTypes} from "../../utils/tacticSort";
import {useTheme} from "@mui/styles";
import {HeadCell} from "./TableCell";
import TacticViewToolbar from "./Toolbar";

const sortTypes = {
    usage: [SortTypes.UsageDown, SortTypes.UsageUp],
    winRate: [SortTypes.WinRateDown, SortTypes.WinRateUp],
    importance: [SortTypes.ImportanceDown, SortTypes.ImportanceUp],
}

const TacticView = inject('analysis')(observer(({analysis}) => {
    const sortType = analysis.sortType;
    const setSortType = analysis.setSortType;
    const sortedTactics = analysis.sortedTactics;
    const isPreviewing = analysis.isPreviewing;
    const [previewTactics, indexOfDeleteTactics, indexOfNewTactics] = analysis.previewTactics;
    const handleSort = types => selection => setSortType(types[selection]);

    const theme = useTheme();

    return <TableContainer id={'tactic-table'}
                           sx={{
                               width: `calc(100% - ${theme.spacing(2)})`,
                               height: `calc(100% - ${theme.spacing(2)})`,
                               m: 1,
                               pr: 1,
                           }}
                           onScroll={console.log}>
        <Table stickyHeader sx={{tableLayout: 'fixed'}}>
            <TableHead>
                <TableRow>
                    <HeadCell label={'No.'} width={isPreviewing ? 120 : 67}/>
                    <HeadCell label={'Tactic'}/>
                    <HeadCell label={'Freq.'} width={120}
                              onSort={handleSort(sortTypes.usage)}
                              sort={sortTypes.usage.indexOf(sortType)}/>
                    <HeadCell label={'Win%'} width={120}
                              onSort={handleSort(sortTypes.winRate)}
                              sort={sortTypes.winRate.indexOf(sortType)}/>
                    <HeadCell label={'Imp.'} width={120}
                              onSort={handleSort(sortTypes.importance)}
                              sort={sortTypes.importance.indexOf(sortType)}/>
                    <HeadCell label={'Pref.'} width={60} noDivider/>
                </TableRow>
            </TableHead>
            <TableBody>
                {isPreviewing ? previewTactics.map((tactic, tId) => (
                    <Tactic key={tId}
                            tactic={tactic}
                            tId={tactic.sortedIndex}
                            selected={analysis.selectedTactics.includes(tactic.fixId)}
                            onSelect={selected => analysis.selectTactic(tactic.fixId, selected)}
                            favorite={analysis.favoriteTactics.includes(tactic.fixId)}
                            onFavorite={favorite => analysis.favoriteTactic(tactic.fixId, favorite)}
                            highlightBackground={tId < indexOfNewTactics}
                            noSelect={tId >= indexOfDeleteTactics && tId < indexOfNewTactics}
                            isPreviewing
                    />
                )) : sortedTactics.map((tactic, tId) => (
                    <Tactic key={tId}
                            tactic={tactic}
                            tId={tId}
                            selected={analysis.selectedTactics.includes(tactic.fixId)}
                            onSelect={selected => analysis.selectTactic(tactic.fixId, selected)}
                            favorite={analysis.favoriteTactics.includes(tactic.fixId)}
                            onFavorite={favorite => analysis.favoriteTactic(tactic.fixId, favorite)}/>
                ))}
            </TableBody>
        </Table>
    </TableContainer>;
}));

export default function useTacticView() {
    return {
        view: <TacticView/>,
        toolbar: <TacticViewToolbar/>,
    }
}
