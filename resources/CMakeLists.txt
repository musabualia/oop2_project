# === Textures ===
configure_file("textures/bomb.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("textures/bullet.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("textures/CoinUI.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("textures/HealthPack.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("textures/level_bg.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("textures/level_btn.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("textures/levelbackground_left.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("textures/levelbackground_middle.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("textures/levelbackground_right.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("textures/Locked_Message.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("textures/menu_background.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("textures/placing_slot.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("textures/robot_bullet.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("textures/select-b.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("textures/select1.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("textures/select2.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("textures/select3.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("textures/select_medkit.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("textures/Sniper_locked.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("textures/SniperBullet.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("textures/slider_knob.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("textures/warningTape.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("textures/welcome_bg.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("textures/btn.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("textures/undo.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("textures/redo.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("textures/back_to_menu.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("textures/select_level.png" ${CMAKE_BINARY_DIR} COPYONLY)

# === Fonts ===
configure_file("fonts/BruceForeverRegular-X3jd2.ttf" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("fonts/BruceForeverRegular.ttf" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("fonts/DejaVuSans.ttf" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("fonts/Orbitron-VariableFont_wght.ttf" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("fonts/SpaceNova-6Rpd1.otf" ${CMAKE_BINARY_DIR} COPYONLY)

# === Config Files ===
configure_file("config/collectibles.cfg" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("config/game.cfg" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("config/physics.cfg" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("config/projectiles.cfg" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("config/robots.cfg" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("config/Settings.cfg" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("config/units.cfg" ${CMAKE_BINARY_DIR} COPYONLY)

# === Base Animations ===
configure_file("animations/coin.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("animations/explosion.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("animations/HealthBag.png" ${CMAKE_BINARY_DIR} COPYONLY)

# === Heavy Gunner Animations ===
configure_file("animations/HeavyGunner_Dead.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("animations/HeavyGunner_Idle.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("animations/HeavyGunner_Shot.png" ${CMAKE_BINARY_DIR} COPYONLY)

# === Rock Robot Animations ===
configure_file("animations/RockRobot_Hit.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("animations/RockRobot_Walk.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("animations/RobotRock_Dead.png" ${CMAKE_BINARY_DIR} COPYONLY)

# === Fire Robot Animations ===
configure_file("animations/fire_robot_death.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("animations/fire_robot_walk.png" ${CMAKE_BINARY_DIR} COPYONLY)

# === Shield Bearer Animations ===
configure_file("animations/ShieldBearer_Block.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("animations/ShieldBearer_Dead.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("animations/ShieldBearer_Idle.png" ${CMAKE_BINARY_DIR} COPYONLY)

# === Sniper Animations ===
configure_file("animations/Sniper_Dead.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("animations/Sniper_Shot.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("animations/ss_Idle.png" ${CMAKE_BINARY_DIR} COPYONLY)

# === Stealth Robot Animations ===
configure_file("animations/StealthRobot_Dead.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("animations/StealthRobot_Hit.png" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("animations/StealthRobot_Walk1.png" ${CMAKE_BINARY_DIR} COPYONLY)


# === OGG Sound Files ===
configure_file("Sounds/game_bg.ogg" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("Sounds/lvl_state.ogg" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("Sounds/Menu1.ogg" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("Sounds/menu_state.ogg" ${CMAKE_BINARY_DIR} COPYONLY)

# === WAV Sound Effects ===
configure_file("Sounds/button_hover.wav" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("Sounds/click.wav" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("Sounds/coin_added.wav" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("Sounds/health_kit_added.wav" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("Sounds/explosion.wav" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("Sounds/placed.wav" ${CMAKE_BINARY_DIR} COPYONLY)


configure_file("Sounds/gameover.wav" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("Sounds/level_up.wav" ${CMAKE_BINARY_DIR} COPYONLY)

configure_file("Sounds/heavy_gunner_shot.wav" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("Sounds/robot_bullet.wav" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("Sounds/sniper_shot.wav" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("Sounds/shield_barier_fight.wav" ${CMAKE_BINARY_DIR} COPYONLY)

configure_file("Sounds/stealth_robot_hit.wav" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("Sounds/rock_robot_hit.wav" ${CMAKE_BINARY_DIR} COPYONLY)

configure_file("Sounds/killed.wav" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("Sounds/squad_died.wav" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("Sounds/rock_robot_die.wav" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("Sounds/shield_died.wav" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("Sounds/Stealth_robot_die.wav" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("Sounds/error_action.wav" ${CMAKE_BINARY_DIR} COPYONLY)
configure_file("Sounds/more.wav" ${CMAKE_BINARY_DIR} COPYONLY)

# === Videos ===
configure_file("videos/more.mp4" ${CMAKE_BINARY_DIR} COPYONLY)


