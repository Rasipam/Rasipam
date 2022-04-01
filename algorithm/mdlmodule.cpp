//
// Created by GuoZiYang on 2022/2/23.
//

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "mdl_module/mdl_enter.h"
#include "base/json.hpp"

using json = nlohmann::json;

static PyObject *
mdl_run(PyObject *self, PyObject *args) {
    MDLParameters arg;
    char * attr_use_str;
    char *insert_patterns_filename;
    int ok = PyArg_ParseTuple(args, "bbsssiiiiisss",
                              &arg.FP_windows,
                              &arg.fill_patterns,
                              &arg.input_filename,
                              &arg.dummy_filename,
                              &arg.output_dir,
                              &arg.minsup,
                              &arg.pattern_window_index_min,
                              &arg.pattern_window_index_max,
                              &arg.pattern_length_min,
                              &arg.pattern_length_max,
                              &attr_use_str,
                              &arg.pattern_file,
                              &insert_patterns_filename);
    if (!ok) return NULL;
    vector<int> attr_use;
    stringstream ss(attr_use_str);
    string use;
    while (ss >> use) {
        attr_use.push_back(stoi(use));
    }
    arg.attr_use = attr_use;
    Pattern::id_top = 0;
    mdl_run(arg, insert_patterns_filename);
    Py_RETURN_NONE;
}

static PyObject *
data_preprocessing(PyObject *self, PyObject *args) {
    int file_type_int = 0;
    char * input_filenames = NULL,
        *input_dir = NULL,
        *output_filename = NULL,
        *processed_sequences_filename = NULL,
        *target_player = NULL,
        *attributes_names_str;
    int ok = PyArg_ParseTuple(args, "issssss",
                              &file_type_int,
                              &target_player,
                              &input_filenames,
                              &input_dir,
                              &output_filename,
                              &processed_sequences_filename,
                              &attributes_names_str);
    if (!ok) return NULL;

    auto file_type = static_cast<FileType>(file_type_int);
    vector<string> files;
    stringstream ss(input_filenames);
    string input_filename;
    while (ss >> input_filename) {
        files.push_back(input_filename);
    }
    vector<string> attribute_names;
    stringstream ss_a(attributes_names_str);
    string attribute_name;
    while (ss_a >> attribute_name) {
        attribute_names.push_back(attribute_name);
    }

    json sequences;
    BaseDS::sequenceList = {};
    BaseDS::load_file(files, file_type, target_player, sequences, attribute_names, input_dir);
    BaseDS::convert_data(output_filename, sequences);

    ofstream processed_sequences_stream;
    processed_sequences_stream.open(processed_sequences_filename);
    processed_sequences_stream << sequences.dump();
    processed_sequences_stream.close();
    Py_RETURN_NONE;
}

static PyObject *
mdl_modify_and_run(PyObject *self, PyObject *args) {
    MDLParameters arg;
    char * attr_use_str;
    char * old_pattern_filename;
    char *delete_patterns_id_str;
    char *insert_patterns_filename;
    int ok = PyArg_ParseTuple(args, "bbsssiiiiisssss",
                                         &arg.FP_windows,
                                         &arg.fill_patterns,
                                         &arg.input_filename,
                                         &arg.dummy_filename,
                                         &arg.output_dir,
                                         &arg.minsup,
                                         &arg.pattern_window_index_min,
                                         &arg.pattern_window_index_max,
                                         &arg.pattern_length_min,
                                         &arg.pattern_length_max,
                                         &attr_use_str,
                                         &arg.pattern_file,
                                         &old_pattern_filename,
                                         &delete_patterns_id_str, &insert_patterns_filename);
    if (!ok) return NULL;

    vector<int> attr_use;
    stringstream ss(attr_use_str);
    string use;
    while (ss >> use) {
        attr_use.push_back(stoi(use));
    }
    arg.attr_use = attr_use;

    vector<int> delete_patterns_id;
    stringstream ss_p(delete_patterns_id_str);
    string pattern_id;
    while (ss_p >> pattern_id) {
        delete_patterns_id.push_back(stoi(pattern_id));
    }
    string old_pattern_filename_str = old_pattern_filename;
    mdl_modify_and_run(old_pattern_filename_str, delete_patterns_id, insert_patterns_filename, arg);
    Py_RETURN_NONE;
}

static PyMethodDef MDLMethods[] = {
        {"run", mdl_run, METH_VARARGS,
         "Run the MDL alg pipeline."},
        {"load_data", data_preprocessing, METH_VARARGS,
         "Preprocess the data."},
        {"modify_and_run", mdl_modify_and_run, METH_VARARGS | METH_KEYWORDS,
         "Modify the MDL model and run."},
        {NULL, NULL, 0, NULL}
};

static struct PyModuleDef mdlmodule = {
        PyModuleDef_HEAD_INIT,
        "mdl_ipam",
        NULL,
        -1,
        MDLMethods
};

PyMODINIT_FUNC
PyInit_mdl_ipam(void) {
    return PyModule_Create(&mdlmodule);
}