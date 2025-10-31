#include "graphics.h"

const struct Background gBackgroundTable[] =
{
	{
	.bgData = "striped_images/backgrounds/wright_co_law_offices/fey_co_murder_night.png",
	.controlBits = BG_MODE_SIZE_480x160 | BG_MODE_HSCROLL_RIGHT | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/wright_co_law_offices/office_day.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/court/defendant_lobby.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/court/defense_bench.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/backgrounds/court/prosecution_bench.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/backgrounds/court/witness_stand.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/backgrounds/court/court_room.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/court/co_counsel.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/court/judge_seat.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/wright_co_law_offices/office_entry_night.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/cutscenes/case2/maya_seeing_dead_mia.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/global_studios/studio_path.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/global_studios/staff_area.png",
	.controlBits = BG_MODE_SIZE_480x160 | BG_MODE_HSCROLL_RIGHT | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/witness_account/mia_dodging_right.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/backgrounds/witness_account/mia_dodging_left.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/backgrounds/evidence/fey_co_diagram.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/backgrounds/witness_account/april_getting_room_service.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/backgrounds/wright_co_law_offices/mia_dead_on_ground.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/cutscenes/case2/phoenix_maya_mia.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/global_studios/studio_2_trailer.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/cutscenes/case2/april_calling_police.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/cutscenes/case2/redd_hitting_mia.png",
	.controlBits = BG_MODE_SIZE_240x320 | BG_MODE_VSCROLL_TOP | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/cutscenes/case2/mia_being_attacked.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/cutscenes/case2/redd_holding_thinker.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/backgrounds/global_studios/will_power_dressing_room.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/global_studios/front_gate.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/global_studios/studio_1.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/gavel/2.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/gavel/3.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/evidence/fey_co_diagram_light.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/backgrounds/detention_center.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/bluecorp_ceo_office.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/gatewater_hotelroom.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/cutscenes/case1/frank_holding_thinker.png",
	.controlBits = BG_MODE_SIZE_240x320 | BG_MODE_VSCROLL_TOP | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/cutscenes/case1/cindy_dead_on_floor.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/cutscenes/case1/cindy_and_frank.png",
	.controlBits = BG_MODE_SIZE_240x320 | BG_MODE_VSCROLL_TOP | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/cutscenes/case1/frank_closeup_1.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/cutscenes/case1/frank_closeup_2.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/cutscenes/case1/frank_closeup_3.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/grossberg_law_offices/grossberg_law_offices_painting.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/grossberg_law_offices/grossberg_law_offices_no_painting.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/witness_account/cindy_finding_frank.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/backgrounds/witness_account/frank_witnessing_larry.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/backgrounds/witness_account/frank_finding_cindy.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/backgrounds/global_studios/studio_2.png",
	.controlBits = BG_MODE_SIZE_480x160 | BG_MODE_HSCROLL_LEFT | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/evidence/maya_phone_call_page_1.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/backgrounds/evidence/maya_phone_call_page_2.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/backgrounds/gavel/1.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/evidence/maya_phone_call_page_3.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/backgrounds/witness_account/jack_at_gate_photo.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/backgrounds/witness_account/jack_dead_at_studio_1.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/backgrounds/witness_account/jack_wearing_samurai_suit.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/backgrounds/witness_account/vasquez_pushing_jack_aftermath.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/backgrounds/witness_account/staff_eating_lunch.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/backgrounds/witness_account/studio_2_jack_and_vasquez.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/backgrounds/witness_account/cody_witnessing_samurai.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/backgrounds/witness_account/samurai_at_studio_2_photo.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/backgrounds/witness_account/5_year_old_photo.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/backgrounds/global_studios/studio_2_trailer_mafia.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/cutscenes/case3/pink_princess.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/global_studios/monkey_on_studio_2_path.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/backgrounds/evidence/global_studio_diagram.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/backgrounds/witness_account/will_power_injury.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/backgrounds/cutscenes/case3/steel_samurai.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "unk",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "unk",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/speedlines.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_SPECIAL_SPEEDLINE | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/courtroom_background.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/capcom_screen.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/steel_samurai_night.png",
	.controlBits = BG_MODE_SIZE_240x320 | BG_MODE_VSCROLL_TOP | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/gourd_lake_entrance.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/gourd_lake_park.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/gourd_lake_park_no_balloon.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/gourd_lake_beach.png",
	.controlBits = BG_MODE_SIZE_480x160 | BG_MODE_HSCROLL_LEFT | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/case3_intro_grass.png",
	.controlBits = BG_MODE_SIZE_240x320 | BG_MODE_NO_SCROLL | BG_MODE_SCROLL_SPECIAL_SLOW | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/case3_intro_trees.png",
	.controlBits = BG_MODE_SIZE_240x320 | BG_MODE_NO_SCROLL | BG_MODE_SCROLL_SPECIAL_SLOW | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/case3_intro_steel_samurai_posing.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/gourd_lake_boat_rental.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/gourd_lake_boat_rental_house.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/police_station_lobby.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/police_station_evidence_storage.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/case3_intro_faceoff.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/backgrounds/evidence/dl6_case_file_page_1.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/backgrounds/evidence/dl6_case_file_page_2.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/backgrounds/evidence/dl6_case_file_page_3.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/case4_train_station.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/case4_gourd_lake_map.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/case4_lottas_photo_zoomed.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/case4_elevator_photo.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/case4_newspaper.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/case4_balloon_launching.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/case4_balloon_landing_in_lake.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/case4_larry_searching_lake.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/case4_class_trial.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/case4_lottas_photo_far.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/case4_class_trial_phoenix.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/case4_class_trial_edgeworth.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/case4_class_trial_larry.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/case4_yanni_threatening.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/case4_shoot_water_from_boat.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/case4_elevator_suffocating.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/case4_elevator_edgeworth_throw.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/case4_elevator_vonkarma_shot.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/case4_elevator_vonkarma_in_door.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/case4_train_station_maya_leaving.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/trial_won.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/case4_intro_lake_one_in_boat.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/case4_intro_threatened_to_shoot.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/case4_intro_edgeworth_picks_gun_up.png",
	.controlBits = BG_MODE_SIZE_240x320 | BG_MODE_VSCROLL_TOP | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/capcom_screen_bw.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_4BPP,
	},
	{
	.bgData = "striped_images/title_screen_demo.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
	{
	.bgData = "striped_images/coming_soon_screen.png",
	.controlBits = BG_MODE_SIZE_240x160 | BG_MODE_NO_SCROLL | BG_MODE_8BPP,
	},
};