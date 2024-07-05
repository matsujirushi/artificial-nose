/*
 * Copyright (c) 2024 EdgeImpulse Inc.
 *
 * Generated by Edge Impulse and licensed under the applicable Edge Impulse
 * Terms of Service. Community and Professional Terms of Service
 * (https://docs.edgeimpulse.com/page/terms-of-service) or Enterprise Terms of
 * Service (https://docs.edgeimpulse.com/page/enterprise-terms-of-service),
 * according to your product plan subscription (the “License”).
 *
 * This software, documentation and other associated files (collectively referred
 * to as the “Software”) is a single SDK variation generated by the Edge Impulse
 * platform and requires an active paid Edge Impulse subscription to use this
 * Software for any purpose.
 *
 * You may NOT use this Software unless you have an active Edge Impulse subscription
 * that meets the eligibility requirements for the applicable License, subject to
 * your full and continued compliance with the terms and conditions of the License,
 * including without limitation any usage restrictions under the applicable License.
 *
 * If you do not have an active Edge Impulse product plan subscription, or if use
 * of this Software exceeds the usage limitations of your Edge Impulse product plan
 * subscription, you are not permitted to use this Software and must immediately
 * delete and erase all copies of this Software within your control or possession.
 * Edge Impulse reserves all rights and remedies available to enforce its rights.
 *
 * Unless required by applicable law or agreed to in writing, the Software is
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing
 * permissions, disclaimers and limitations under the License.
 */

#ifndef _EI_CLASSIFIER_ANOMALY_METADATA_H_
#define _EI_CLASSIFIER_ANOMALY_METADATA_H_

#include "edge-impulse-sdk/classifier/ei_model_types.h"

const uint16_t ei_classifier_anom_axes[] = { 3, 8, 13, 18 };

// (before - mean) / scale
const float ei_classifier_anom_scale[4] = { 0.007925100653295112, 0.06865409454983797, 0.07559002381172308, 0.06847339683105355 };
const float ei_classifier_anom_mean[4] = { 0.3945722571910827, 0.513884902152004, 0.4772689125627419, 0.5138270487784407 };
const ei_classifier_anom_cluster_t ei_classifier_anom_clusters[12] = {
	{ ( float[4] ) { 1.6308045387268066, -0.8750964403152466, -0.8342728018760681, -0.8761033415794373 }, 0.4729620568111176 },
	{ ( float[4] ) { 0.9983583688735962, -0.8375537991523743, -0.782543957233429, -0.8380146622657776 }, 0.48035651518875755 },
	{ ( float[4] ) { -0.3683980405330658, -0.35748177766799927, -0.37487533688545227, -0.35672906041145325 }, 0.41633883430136287 },
	{ ( float[4] ) { -1.0709648132324219, 2.5425338745117188, 2.643224000930786, 2.545287847518921 }, 0.5191881600094476 },
	{ ( float[4] ) { -0.6397583484649658, 0.44050559401512146, 0.3485764265060425, 0.4454713761806488 }, 0.23599651442525477 },
	{ ( float[4] ) { 0.12467727065086365, -0.2491482049226761, -0.31763848662376404, -0.24674446880817413 }, 0.43094032139479893 },
	{ ( float[4] ) { -1.3251286745071411, 1.8259552717208862, 1.8268969058990479, 1.8245218992233276 }, 0.44057226369452074 },
	{ ( float[4] ) { -0.7309852838516235, 0.012390080839395523, -0.03487902507185936, 0.015715772286057472 }, 0.41791124727529794 },
	{ ( float[4] ) { -1.5726425647735596, 1.2435736656188965, 1.2202067375183105, 1.2387908697128296 }, 0.39843504579879524 },
	{ ( float[4] ) { -0.04223250970244408, 3.024589776992798, 3.154625415802002, 3.029411554336548 }, 0.1841064448347057 },
	{ ( float[4] ) { 0.5771950483322144, -0.6279383897781372, -0.618504524230957, -0.628158450126648 }, 0.5458264241303803 },
	{ ( float[4] ) { -1.3340086936950684, 0.6787558197975159, 0.6354657411575317, 0.6703510880470276 }, 0.3752562199198646 },
};

#endif // _EI_CLASSIFIER_ANOMALY_METADATA_H_