import os.path
import pdb
from copy import deepcopy
from typing import List, Optional
from fastapi import FastAPI, Request
from fastapi.middleware.cors import CORSMiddleware
from starlette.responses import StreamingResponse, JSONResponse

from src.data_manager import data_manager
from src.mine_alg_interface import MineAlgInterface
from src.model import DatasetInfo, SequenceFilter, TacticSet, Rally, Modification, global_modification_types
from src.model.rally import RallyDetail
from src.tactic_dimensionality_reducer import TacticDimensionalityReducer
from src.utils import gen_token, video_file
from src.utils.common import *
from src.utils.token import auth_required, get_token_from_request
from src.utils.utils import GlobalConstrains, TimeSpot, find_tactic_id, find_tactic, split_tactic, merge_tactic, \
    increment_hit, decrement_hit, modify_value

app = FastAPI()

origins = [
    "http://localhost:3000",
]

app.add_middleware(
    CORSMiddleware,
    allow_origins=origins,
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)


# 0. 确认后端运行状态
@app.get("/")
async def root():
    return {"message": "Hello World"}


# 1. 申请token
@app.get('/token', response_model=str)
async def get_token():
    token = gen_token()
    data_manager.init_token(token)
    return token


# 2. 获取数据集列表
@app.get("/datasets", response_model=List[DatasetInfo])
async def get_datasets():
    return data_manager.get_datasets()


# 3. 指定数据集
@app.post('/dataset', response_model=bool)
@auth_required
async def set_dataset(request: Request, sequence_filter: SequenceFilter):
    token = get_token_from_request(request)
    new_metadata = data_manager.filter_matches(sequence_filter)
    data_manager.update_metadata(token, new_metadata)
    return True


# 4. 运算
@app.post('/tactic', response_model=TacticSet)
@auth_required
async def cal_tactic(request: Request):
    token = get_token_from_request(request)
    metadata = data_manager.get_metadata(token)
    preference_tactics = data_manager.get_preference_tactics(token)
    global_constrains = GlobalConstrains()

    mine_alg_pickle_path = os.path.join(DATA_DIR, metadata['store_dir'], metadata['store_dir'] + '.pkl')
    mine_alg_interface = MineAlgInterface(**metadata, token=token)
    mine_alg_interface.reset_version()
    mine_alg_interface.data_preprocess()
    mine_alg_interface.run(global_constrains)


    rallies = mine_alg_interface.get_rallies()
    tactics = mine_alg_interface.get_tactics()
    desc_len = tactics['desc_len']

    tactic_dim_reducer_bin_dir = os.path.join(DATA_DIR, metadata['store_dir'])
    tactic_dim_reducer = TacticDimensionalityReducer.load(tactic_dim_reducer_bin_dir)
    coordinates = tactic_dim_reducer.fit_transform(tactics)
    tactic_dim_reducer.save(tactic_dim_reducer_bin_dir)

    tactics = TacticSet.load(tactics, rallies, coordinates, desc_len, preference_tactics)
    data_manager.update_tactic_set(token, tactics)
    data_manager.update_global_constrains(token, global_constrains)
    data_manager.update_timeline(token, TimeSpot.InitRun, 0)
    # mine_alg_interface.update_version()
    mine_alg_interface.save(mine_alg_pickle_path)
    return tactics

debug_flag = 1
# 5. 获取回合
@app.get('/rally/{tac_id}', response_model=List[Rally])
@auth_required
async def get_rally(request: Request, tac_id: str):
    token = get_token_from_request(request)
    metadata = data_manager.get_metadata(token)

    mine_alg_pickle_path = os.path.join(DATA_DIR, metadata['store_dir'], metadata['store_dir'] + '.pkl')
    mine_alg_interface = MineAlgInterface.load(mine_alg_pickle_path,
                                               params={**metadata, 'token': token})
    mine_alg_interface.set_new_store_path()
    # if debug_flag == 0:
    #     pdb.set_trace()
    tactic_set = data_manager.get_tactic_set(token)

    rallies = mine_alg_interface.get_rallies()
    tactics = mine_alg_interface.get_tactics()

    rallies = Rally.find_for_tactic(tac_id, tactic_set, tactics, rallies)
    return rallies


# 6. 获取回合细节
# @app.get('/rally/detail/{rally_id}', response_model=RallyDetail)
# @auth_required
# async def get_rally(request: Request, rally_id: int):
#     token = get_token_from_request(request)
#     # TODO: return rallies that used the tactic with id `tac_id`
#     detail = {
#         "attr": [],
#         "hits": [],
#     }
#     return detail


# 7. 文本处理
@app.get('/text/{t}', response_model=Optional[Modification])
@auth_required
async def process_text(request: Request, t: str):
    # TODO: 返回文本处理结果
    return None


# 8. 增加修改
@app.post('/modification', response_model=TacticSet)
@auth_required
async def cal_tactic(request: Request, modification: Modification):
    token = get_token_from_request(request)
    metadata = data_manager.get_metadata(token)
    global_constrains = deepcopy(data_manager.get_global_constrains(token))
    preference_tactics = data_manager.get_preference_tactics(token)
    mine_alg_pickle_path = os.path.join(DATA_DIR, metadata['store_dir'], metadata['store_dir'] + '.pkl')
    mine_alg_interface = MineAlgInterface.load(mine_alg_pickle_path,
                                               params={**metadata, 'token': token})
    old_tactics = mine_alg_interface.get_tactics()
    old_rallies = mine_alg_interface.get_rallies()
    tactic_set = data_manager.get_tactic_set(token)
    mine_alg_interface.update_version()
    mine_alg_interface.set_new_store_path()
    mine_alg_interface.data_preprocess()

    delete_tactics_id = []
    insert_tactics = []
    # TacticSet
    if modification.type == 'LimitIndex':
        global_constrains.set_pattern_window_index_range(
            int(modification.params['min']) if 'min' in modification.params else global_constrains.index_min,
            int(modification.params['max']) if 'max' in modification.params else global_constrains.index_max)
    elif modification.type == 'LimitLength':
        global_constrains.set_pattern_length_range(
            int(modification.params['min']) if 'min' in modification.params else global_constrains.length_min,
            int(modification.params['max']) if 'max' in modification.params else global_constrains.length_max)
        if 'offset' in modification.params:
            global_constrains.update_pattern_length_range(int(modification.params['offset']))
    # Attribute
    elif modification.type == 'SetExistence':
        if 'attr' not in modification.params:
            return JSONResponse(
                status_code=401,
                content={"detail": "[Modification]Attribute set existence: no attribute passed."}
            )
        exist = modification.params['exist'] if 'exist' in modification.params else False
        attributes = metadata["attributes"]
        attr_id = [i for i in range(len(attributes)) if attributes[i] == modification.params['attr']][0]
        global_constrains.set_exist_attribute(attr_id, exist)
    elif modification.type == 'SetImportance':
        if 'attr' not in modification.params:
            return JSONResponse(
                status_code=401,
                content={"detail": "[Modification]Attribute set importance: no attribute passed."}
            )
        # TODO: normalize the importance in [0,10]
        attributes = metadata["attributes"]
        attr_id = [i for i in range(len(attributes)) if attributes[i] == modification.params['attr']][0]
        global_constrains.set_attr_use(attr_id, int(modification.params['importance']))
    # Tactic
    elif modification.type == 'Delete':
        if 'index' not in modification.params:
            return JSONResponse(
                status_code=401,
                content={"detail": "[Modification]Tactic delete: no tactic index passed."}
            )
        tactic_id = find_tactic_id(old_tactics, tactic_set, modification.params['index'])
        if isinstance(tactic_id, list):
            delete_tactics_id = tactic_id
        else:
            delete_tactics_id.append(tactic_id)
    elif modification.type == 'Split':
        if 'index' not in modification.params:
            return JSONResponse(
                status_code=401,
                content={"detail": "[Modification]Tactic split: no tactic index passed."}
            )
        tactic_id = find_tactic_id(old_tactics, tactic_set, modification.params['index'])[0]
        delete_tactics_id.append(tactic_id)
        tactic = find_tactic(tactic_set, modification.params['index'])[0]
        tactic_surrounding = tactic.tactic_surrounding
        attr_id = -1
        if 'attr' in modification.params:
            attributes = metadata["attributes"]
            attr_id = [i for i in range(len(attributes)) if attributes[i] == modification.params['attr']][0]
        hit = -1 if 'hit' not in modification.params else int(modification.params['hit'])
        insert_tactics = split_tactic(tactic.tactic, tactic_surrounding, attr_id, hit)
    elif modification.type == 'Merge':
        if 'index' not in modification.params:
            return JSONResponse(
                status_code=401,
                content={"detail": "[Modification]Tactic merge: no tactic index passed."}
            )
        tactic_ids = find_tactic_id(old_tactics, tactic_set, modification.params['index'])
        tactics = find_tactic(tactic_set, modification.params['index'])
        if not isinstance(tactics, list):
            return JSONResponse(
                status_code=401,
                content={"detail": "[Modification]Tactic merge: multiple tactic indices must be passed."}
            )
        delete_tactics_id += tactic_ids
        insert_tactics = merge_tactic(tactics)
    # Hit
    elif modification.type == 'Increment' or modification.type == 'Decrement':
        if 'index' not in modification.params:
            return JSONResponse(
                status_code=401,
                content={"detail": "[Modification]Hit: no tactic index passed."}
            )
        if 'direction' not in modification.params:
            return JSONResponse(
                status_code=401,
                content={"detail": "[Modification]Hit: no direction passed."}
            )
        if 'hitCount' not in modification.params:
            return JSONResponse(
                status_code=401,
                content={"detail": "[Modification]Hit: no hit count passed."}
            )
        # pdb.set_trace()
        tactic_id = find_tactic_id(old_tactics, tactic_set, modification.params['index'])[0]
        delete_tactics_id.append(tactic_id)
        tactic = find_tactic(tactic_set, modification.params['index'])[0]
        if modification.type == 'Increment':
            insert_tactics = increment_hit(tactic, old_rallies, int(modification.params['direction']),
                                           int(modification.params['hitCount']))
        else:
            insert_tactics = decrement_hit(tactic, int(modification.params['direction']),
                                           int(modification.params['hitCount']))
    # Value
    elif modification.type == 'Replace' or modification.type == 'Ignore' or modification.type == 'Explore':
        if 'index' not in modification.params:
            return JSONResponse(
                status_code=401,
                content={"detail": "[Modification]Value: no tactic index passed."}
            )
        if 'attr' not in modification.params:
            return JSONResponse(
                status_code=401,
                content={"detail": "[Modification]Value: no attribute passed."}
            )
        if 'hit' not in modification.params:
            return JSONResponse(
                status_code=401,
                content={"detail": "[Modification]Value: no hit passed."}
            )
        if modification.type == 'Replace' and 'target' not in modification.params:
            return JSONResponse(
                status_code=401,
                content={"detail": "[Modification]Value replace: no target value passed."}
            )
        tactic_id = find_tactic_id(old_tactics, tactic_set, modification.params['index'])[0]
        delete_tactics_id.append(tactic_id)
        tactic = find_tactic(tactic_set, modification.params['index'])[0]

        attributes = metadata["attributes"]
        attr_id = [i for i in range(len(attributes)) if attributes[i] == modification.params['attr']][0]
        if modification.type == 'Replace':
            insert_tactics = modify_value(tactic, old_rallies,
                                          int(modification.params['hit']),
                                          attr_id,
                                          modification.type,
                                          modification.params['target'])
        else:
            insert_tactics = modify_value(tactic, old_rallies,
                                          int(modification.params['hit']),
                                          attr_id,
                                          modification.type)

    insert_tactics += [tac.tactic for tac in preference_tactics]
    if modification.type in global_modification_types:
        mine_alg_interface.run(global_constrains, insert_tactics=insert_tactics)
    else:
        old_store_path = mine_alg_interface.get_store_path(str(mine_alg_interface.get_version() - 1))
        old_tactics_path = os.path.join(old_store_path, OUTPUT_DIR, PATTERN_FILE)
        mine_alg_interface.modify_and_run(delete_tactics_id, insert_tactics, old_tactics_path, global_constrains)


    rallies = mine_alg_interface.get_rallies()
    tactics = mine_alg_interface.get_tactics()

    desc_len = tactics['desc_len']

    tactic_dim_reducer_bin_dir = os.path.join(DATA_DIR, metadata['store_dir'])
    tactic_dim_reducer = TacticDimensionalityReducer.load(tactic_dim_reducer_bin_dir)
    if modification.type in global_modification_types:
        coordinates = tactic_dim_reducer.fit_transform(tactics)
        tactic_dim_reducer.save(tactic_dim_reducer_bin_dir)
    else:
        coordinates = tactic_dim_reducer.transform(tactics)
    if modification.type in global_modification_types:
        tactics = TacticSet.load(tactics, rallies, coordinates, desc_len, preference_tactics)
    else:
        tactics = TacticSet.load(tactics, rallies, coordinates, desc_len, preference_tactics,
                                 last_tactics=tactic_set.tactics)
    data_manager.update_tactic_set(token, tactics)
    if modification.type in global_modification_types:
        data_manager.update_global_constrains(token, global_constrains)
    if modification.type in global_modification_types:
        data_manager.update_timeline(token, TimeSpot.GlobalModification, mine_alg_interface.get_version() - 1)
    else:
        data_manager.update_timeline(token, TimeSpot.LocalModification, mine_alg_interface.get_version() - 1)

    # mine_alg_interface.update_version()
    mine_alg_interface.save(mine_alg_pickle_path)
    return tactics


# 9. 撤销修改
@app.delete('/modification', response_model=bool)
@auth_required
async def cal_tactic(request: Request):
    global debug_flag
    debug_flag = 0
    token = get_token_from_request(request)
    time_line = data_manager.get_timeline(token)
    time_spot = time_line['time_spot']
    last_mdl_version = time_line['last_version']

    metadata = data_manager.get_metadata(token)
    mine_alg_pickle_path = os.path.join(DATA_DIR, metadata['store_dir'], metadata['store_dir'] + '.pkl')
    mine_alg_interface = MineAlgInterface.load(mine_alg_pickle_path,
                                               params={**metadata, 'token': token})
    if time_spot == TimeSpot.GlobalModification:
        mine_alg_interface.set_version(last_mdl_version)
        data_manager.undo_tactic_set(token)
        data_manager.undo_global_constrains(token)
        data_manager.undo_timeline(token)
        mine_alg_interface.save(mine_alg_pickle_path)
    elif time_spot == TimeSpot.LocalModification:
        mine_alg_interface.set_version(last_mdl_version)
        data_manager.undo_tactic_set(token)
        data_manager.undo_timeline(token)
        mine_alg_interface.save(mine_alg_pickle_path)
    else:
        return False
    return True


# 10. 是否固定战术
@app.put('/tactic/preference/{tac_id}', response_model=bool)
@auth_required
async def fix_tactic(request: Request, tac_id: str, preference: bool):
    token = get_token_from_request(request)
    tactic_set = data_manager.get_tactic_set(token)
    tactic = find_tactic(tactic_set, tac_id)
    if preference and not data_manager.find_preference_tactics(token, tactic):
        data_manager.add_preference_tactics(token, tactic)
    elif not preference and data_manager.find_preference_tactics(token, tactic):
        data_manager.delete_preference_tactics(token, tactic)
    else:
        return False
    return True


# 11. 视频
@app.get('/video/{video_name}')
def get_video(video_name: str):
    try:
        return StreamingResponse(video_file(video_name), media_type="video/mp4")
    except:
        return JSONResponse(
            status_code=404,
            content={"detail": "Video is not found"}
        )
