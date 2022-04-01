import Toolbar from "../Common/Toolbar";
import styled from "@emotion/styled";
import {winColors} from "../../static/theme";
import {Box, Radio, Stack, Tooltip} from "@mui/material";
import {Info} from "@mui/icons-material";
import winRate2color from "../../utils/winRate2color";


const ProjectViewToolbar = ({sizeEncoding, setSizeEncoding}) => <Toolbar elements={[
    <Tooltip disableInteractive={false}
             title={<table>
                 <tbody>
                 <tr>
                     <td>
                         <Box display={'flex'} alignItems={'center'} justifyContent={'center'}>
                             <Circle style={{width: 15, height: 15, margin: 5}}/>
                             <Circle/>
                         </Box>
                     </td>
                     <td>
                         <Stack direction={'row'}>
                             <Box>
                                 <Radio size={'small'}
                                        checked={sizeEncoding === 'freq'}
                                        onClick={() => setSizeEncoding('freq')}/>
                                 Freq.
                             </Box>
                             <Box>
                                 <Radio size={'small'}
                                        checked={sizeEncoding === 'imp'}
                                        onClick={() => setSizeEncoding('imp')}/>
                                 Imp.
                             </Box>
                         </Stack>
                     </td>
                 </tr>
                 <tr>
                     <td>
                         <Box display={'flex'} alignItems={'center'} justifyContent={'center'}>
                             <div style={{
                                 width: '100%',
                                 height: 15,
                                 background: `linear-gradient(to right, ${winRate2color(0)}, ${winRate2color(0.5)} 50%, ${winRate2color(1)})`,
                             }}/>
                         </Box>
                     </td>
                     <td>Win%: 0% ~ 100%</td>
                 </tr>
                 <tr>
                     <td>
                         <Box display={'flex'} alignItems={'center'} justifyContent={'center'}>
                             <Circle style={{border: '2px solid black'}}/>
                             <Circle style={{border: '2px solid black', backgroundColor: winColors[1]}}/>
                         </Box>
                     </td>
                     <td>Original tactics</td>
                 </tr>
                 <tr>
                     <td>
                         <Box display={'flex'} alignItems={'center'} justifyContent={'center'}>
                             <Circle style={{border: '2px dashed black'}}/>
                             <Circle style={{border: '2px dashed black', backgroundColor: winColors[1]}}/>
                         </Box>
                     </td>
                     <td>New tactics</td>
                 </tr>
                 </tbody>
             </table>}>
        <Info/>
    </Tooltip>
]}/>;

const Circle = styled('div')({
    borderRadius: '50%',
    width: 25,
    height: 25,
    backgroundColor: winColors[0],
})

export default ProjectViewToolbar;
