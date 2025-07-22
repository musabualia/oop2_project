file (GLOB_RECURSE MY_SOURCE_FILES CONFIGURE_DEPENDS LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.cpp)
target_sources (${CMAKE_PROJECT_NAME} PRIVATE ${MY_SOURCE_FILES}
"../include/Core/GameObject.h" 
"../include/Core/MovingObject.h"
"../include/Core/StaticObject.h" 
"../include/Core/Game.h" 
"../include/Core/GameManager.h"
"../include/Core/Constants.h" 
"../include/Core/Timer.h"

"../include/Entities/Base/Robot.h"
"../include/Entities/Base/SquadMember.h"
"../include/Entities/Base/Projectile.h" 
"../include/Entities/Base/Collectible.h"

"../include/Entities/Robots/BasicRobot.h"
"../include/Entities/Robots/FireRobot.h" 
 

"../include/Entities/Robots/StealthRobot.h"

"../include/Entities/SquadMembers/HeavyGunnerMember.h"
"../include/Entities/SquadMembers/SniperMember.h"
"../include/Entities/SquadMembers/ShieldBearerMember.h" 


"../include/Entities/Projectiles/Bullet.h" 
 
"../include/Entities/Projectiles/SniperBullet.h" 
 

"../include/Entities/Collectibles/Coin.h" 
 
"../include/Entities/Collectibles/HealthPack.h"


 


"../include/Managers/RobotManager.h" 
"../include/Managers/SquadMemberManager.h"
"../include/Managers/ProjectileManager.h"  
"../include/Managers/WaveManager.h"
"../include/Managers/AudioManager.h"
"../include/Managers/ResourceManager.h"  
"../include/Managers/SettingsManager.h"
"../include/Managers/HighScoreManager.h"
"../include/Managers/EntityManager.h" 

"../include/States/IState.h"
"../include/States/StateMachine.h" 
"../include/States/MenuState.h"
"../include/States/PlayState.h"
"../include/States/PauseState.h" 
"../include/States/GameOverState.h" 
"../include/States/VictoryState.h"
"../include/States/SettingsState.h"

"../include/UI/HUD.h"
"../include/UI/Button.h" 



"../include/Systems/EventSystem.h" 
 
"../include/Systems/AnimationSystem.h"

"../include/Factories/RobotFactory.h"
"../include/Factories/SquadMemberFactory.h"
"../include/Factories/ProjectileFactory.h"
"../include/Factories/CollectibleFactory.h" 


"../include/Physics/PhysicsUtils.h" 
"../include/Physics/CollisionCategories.h" 
"../include/Physics/PhysicsContactListener.h"
"../include/Physics/PhysicsWorld.h" 

"../include/Utils/ConfigLoader.h" 

"Core/GameObject.cpp"
"Core/MovingObject.cpp"
"Core/StaticObject.cpp" 
"Core/Game.cpp" 
"Core/GameManager.cpp"
"Core/Timer.cpp" 

"Entities/Base/Robot.cpp" 
"Entities/Base/SquadMember.cpp" 
"Entities/Base/Projectile.cpp"
"Entities/Base/Collectible.cpp" 

"Entities/Robots/BasicRobot.cpp"
"Entities/Robots/FireRobot.cpp" 


"Entities/Robots/StealthRobot.cpp" 

"Entities/SquadMembers/HeavyGunnerMember.cpp" 
"Entities/SquadMembers/SniperMember.cpp"
"Entities/SquadMembers/ShieldBearerMember.cpp" 


"Entities/Projectiles/Bullet.cpp"

"Entities/Projectiles/SniperBullet.cpp" 


"Entities/Collectibles/Coin.cpp"
 
 


"Managers/RobotManager.cpp" 
"Managers/SquadMemberManager.cpp"
"Managers/ProjectileManager.cpp"
"Managers/WaveManager.cpp" 
"Managers/AudioManager.cpp" 
"Managers/ResourceManager.cpp" 
 

"States/StateMachine.cpp" 
"States/MenuState.cpp" 
"States/PlayState.cpp"
"States/PauseState.cpp" 
"States/GameOverState.cpp" 
"States/VictoryState.cpp" 
"States/SettingsState.cpp" 

"UI/HUD.cpp" "UI/Button.cpp"  

 
"Systems/AnimationSystem.cpp" 
"Systems/EventSystem.cpp" 
 

"Factories/RobotFactory.cpp" 
"Factories/SquadMemberFactory.cpp"
"Factories/ProjectileFactory.cpp"
"Factories/CollectibleFactory.cpp"  

"Utils/ConfigLoader.cpp"

 
"Physics/PhysicsWorld.cpp" 
"Physics/PhysicsContactListener.cpp"
"Managers/SettingsManager.cpp"
"../include/UI/GridRenderer.h"
"UI/GridRenderer.cpp" 

"../include/States/SplashState.h" 
"States/SplashState.cpp" 
 "../include/Entities/Projectiles/RobotBullet.h" 
 "Entities/Projectiles/RobotBullet.cpp" 
 "../include/Commands/ICommand.h" "../include/Commands/PlaceUnitCommand.h" "../include/Commands/CommandManager.h" "Commands/PlaceUnitCommand.cpp" "Commands/CommandManager.cpp" "../include/Commands/PlaceBombCommand.h" "Commands/PlaceBombCommand.cpp" "Managers/DynamicWaveGenerator.cpp" "../include/Managers/DynamicWaveGenerator.h" 
 "Entities/Collectibles/HealthPack.cpp")
