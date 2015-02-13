// Author: Jiesi Zhao (jiesizhao0423@gmail.com), Wei Dai (wdai@cs.cmu.edu)
// Date: 2014.11.5

#include "split_finder.hpp"
#include "utils.hpp"
#include <algorithm>
#include <glog/logging.h>
#include <limits>
#include <random>
#include <math.h>

namespace tree {

SplitFinder::SplitFinder(int32_t num_labels) :
  num_labels_(num_labels), pre_split_entropy_(0.) { }

void SplitFinder::Reset(int32_t num_labels) {
	num_labels_ = num_labels;
	pre_split_entropy_ = 0.0;
	entries_.clear();
}

void SplitFinder::AddInstance(float feature_val, int32_t label,
    float weight) {
  FeatureEntry new_entry;
  new_entry.feature_val = feature_val;
  new_entry.label = label;
  new_entry.weight = weight;
  entries_.push_back(new_entry);
}

void SplitFinder::AddInstanceDedup(float feature_val,
    int32_t label, float weight) {
  // To be optimized
  for (auto iter = entries_.begin(); iter != entries_.end(); ++iter) {
    if ((iter->feature_val == feature_val)
        && (iter->label == label)) {
      iter->weight += weight;
      return;
    }
  }

  // This is a new feature_val-label combo.
  AddInstance(feature_val, label, weight);
}

float SplitFinder::FindSplitValue(float* gain_ratio) {
  SortEntries();
  // Compute entropy of label
  std::vector<float> label_distribution(num_labels_, 0);
  for (auto iter = entries_.begin(); iter != entries_.end(); ++iter) {
    label_distribution[iter->label] += 1;
  }
  Normalize(&label_distribution);
  pre_split_entropy_ = ComputeEntropy(label_distribution);

  float min_value = entries_.begin()->feature_val;
  float max_value = entries_.back().feature_val;

  // Find distinct feature values
  std::vector<float> feature_value_dist;
  feature_value_dist.push_back(min_value);
  if (max_value > min_value) { 
    for (int i = 1; i < entries_.size(); i++) {
      if (entries_[i].feature_val > feature_value_dist.back()) {
        feature_value_dist.push_back(entries_[i].feature_val);
      }
    }    
  }

  std::random_device rd;
  std::mt19937 mt(rd());

  float best_gain_ratio = std::numeric_limits<float>::min();
  float best_split_val = min_value;
  std::vector<float> left_dist(num_labels_);
  std::vector<float> right_dist(num_labels_);
  float left_dist_weight = 0.;
  float right_dist_weight = 0.;
  int idx_start = 0;
  // all to right first
  for (auto iter = entries_.begin(); iter != entries_.end(); iter++) {
	right_dist[iter->label] += iter->weight;
	right_dist_weight += iter->weight;
  }

  for (int i = 1; i < feature_value_dist.size(); ++i) {
	  std::uniform_real_distribution<float> dist(feature_value_dist[i-1], feature_value_dist[i]);
      // Randomly generate a split threshold
      float rand_split = dist(mt);
      float gain_ratio = ComputeGainRatio(left_dist, right_dist, 
			  left_dist_weight, right_dist_weight, idx_start, 
			  rand_split);

      if (gain_ratio > best_gain_ratio) {
        best_gain_ratio = gain_ratio;
        best_split_val = rand_split;
      }
    }
  if (gain_ratio != 0) {
    *gain_ratio = best_gain_ratio;
  }
  return best_split_val;
}

// ================== Private Functions ===============

void SplitFinder::SortEntries() {
  std::sort(entries_.begin(), entries_.end(),
      [] (const FeatureEntry& i, const FeatureEntry& j) {
      return (i.feature_val < j.feature_val ||
        (i.feature_val == j.feature_val && i.label < j.label)); });
}

float SplitFinder::ComputeGainRatio(std::vector<float>& left_dist, std::vector<float>& right_dist,
		float& left_dist_weight, float& right_dist_weight, int32_t& idx,
		float split_val) {
  //std::vector<float> left_dist(num_labels_);    // left distribution.
  //std::vector<float> right_dist(num_labels_);
  //float left_dist_weight = 0.;
  //float right_dist_weight = 0.;
  std::vector<float> left_dist_copy;
  std::vector<float> right_dist_copy;
  FeatureEntry fe;

  for (; idx < entries_.size(); idx++) {
	fe = entries_[idx];
	if (fe.feature_val <= split_val) {
		left_dist[fe.label] += fe.weight;
		right_dist[fe.label] -= fe.weight;
		left_dist_weight += fe.weight;
		right_dist_weight -= fe.weight;
	} else {
		break;
	}
  }

  //for (auto iter = entries_.begin(); iter != entries_.end(); ++iter) {
    //if (iter->feature_val <= split_val) {
      //left_dist[iter->label] += iter->weight;
      //left_dist_weight += iter->weight;
    //} else {
      //right_dist[iter->label] += iter->weight;
      //right_dist_weight += iter->weight;
    //}
  //}

  left_dist_copy = left_dist;
  right_dist_copy = right_dist;

  // Normalize
  Normalize(&left_dist_copy);
  Normalize(&right_dist_copy);

  // Compute entropy
  float left_entropy = ComputeEntropy(left_dist_copy);
  float right_entropy = ComputeEntropy(right_dist_copy);

  // Compute conditional entropy
  std::vector<float> split_dist;
  split_dist.push_back(left_dist_weight /
      (left_dist_weight + right_dist_weight));
  split_dist.push_back(right_dist_weight /
      (left_dist_weight + right_dist_weight));
  float cond_entropy = split_dist[0] * left_entropy +
    split_dist[1] * right_entropy;

  // information gain
  float info_gain = pre_split_entropy_ - cond_entropy;
  Normalize(&split_dist);
  float splitinfo = ComputeEntropy(split_dist);
  float gain_ratio = info_gain / splitinfo;
  if (splitinfo == 0.0) {
    gain_ratio = 0.0;
  }
  return gain_ratio;
}

}  // namespace tree
