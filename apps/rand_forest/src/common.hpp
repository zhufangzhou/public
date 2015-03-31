// Author: Dai Wei (wdai@cs.cmu.edu)
// Date: 2014.11.06

#pragma once

#include <gflags/gflags.h>

// Petuum Parameters
DECLARE_int32(num_clients);
DECLARE_int32(num_app_threads);
DECLARE_int32(client_id);
DECLARE_int32(num_comm_channels_per_client);

// Data Parameters
DECLARE_int32(num_train_data);
DECLARE_int32(num_train_data_eval);
DECLARE_string(train_file);
DECLARE_bool(global_data);
DECLARE_string(test_file);
DECLARE_bool(perform_test);
DECLARE_bool(compute_importance);

// Rand Forest Parameters
DECLARE_int32(test_vote_table_id);
DECLARE_int32(train_vote_table_id);
DECLARE_int32(gain_ratio_table_id);
DECLARE_int32(train_intermediate_table_id);
DECLARE_int32(test_intermediate_table_id);
DECLARE_int32(num_trees);
DECLARE_int32(max_depth);
DECLARE_int32(num_data_subsample);
DECLARE_int32(num_features_subsample);
DECLARE_int32(num_layers);

// Save and Load
DECLARE_bool(save_pred);
DECLARE_bool(output_proba);
DECLARE_bool(save_report);
DECLARE_string(report_file);
DECLARE_string(pred_file);
DECLARE_bool(save_trees);
DECLARE_string(output_file);
DECLARE_bool(load_trees);
DECLARE_string(input_file);
