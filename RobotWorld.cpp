#include "RobotWorld.hpp"
#include "Logger.hpp"
#include "Robot.hpp"
#include "WayPoint.hpp"
#include "Goal.hpp"
#include "Wall.hpp"
#include "MainApplication.hpp"
#include <algorithm>
#include "CommunicationService.hpp"
#include "Client.hpp"
#include "Message.hpp"

namespace Model
{
	/**
	 *
	 */
	/* static */RobotWorld& RobotWorld::RobotWorld::getRobotWorld()
	{
		static RobotWorld robotWorld;
		return robotWorld;
	}
	/**
	 *
	 */
	RobotPtr RobotWorld::newRobot(	const std::string& aName /*= "New Robot"*/,
									const Point& aPosition /*= Point(-1,-1)*/,
									bool aNotifyObservers /*= true*/)
	{
		RobotPtr robot( new Robot( aName, aPosition));
		robots.push_back( robot);
		if (aNotifyObservers == true)
		{
			notifyObservers();
		}
		return robot;
	}
	/**
	 *
	 */
	WayPointPtr RobotWorld::newWayPoint(	const std::string& aName /*= "new WayPoint"*/,
											const Point& aPosition /*= Point(-1,-1)*/,
											bool aNotifyObservers /*= true*/)
	{
		WayPointPtr wayPoint(new WayPoint( aName, aPosition));
		wayPoints.push_back( wayPoint);
		if (aNotifyObservers == true)
		{
			notifyObservers();
		}
		return wayPoint;
	}
	/**
	 *
	 */
	GoalPtr RobotWorld::newGoal(	const std::string& aName /*= "New Goal"*/,
									const Point& aPosition /*= Point(-1,-1)*/,
									bool aNotifyObservers /*= true*/)
	{
		GoalPtr goal( new Goal( aName, aPosition));
		goals.push_back( goal);
		if (aNotifyObservers == true)
		{
			notifyObservers();
		}
		return goal;
	}
	/**
	 *
	 */
	WallPtr RobotWorld::newWall(const Point& aPoint1,
								const Point& aPoint2,
								bool aNotifyObservers /*= true*/)
	{
		WallPtr wall( new Wall( aPoint1, aPoint2));
		walls.push_back( wall);
		if (aNotifyObservers == true)
		{
			notifyObservers();
		}
		return wall;
	}
	/**
	 *
	 */
	void RobotWorld::deleteRobot( 	RobotPtr aRobot,
									bool aNotifyObservers /*= true*/)
	{
		auto i = std::find_if( robots.begin(), robots.end(), [aRobot](RobotPtr r)
							   {
									return aRobot->getName() == r->getName();
							   });
		if (i != robots.end())
		{
			robots.erase( i);
			if (aNotifyObservers == true)
			{
				notifyObservers();
			}
		}
	}
	/**
	 *
	 */
	void RobotWorld::deleteWayPoint( 	WayPointPtr aWayPoint,
										bool aNotifyObservers /*= true*/)
	{
		auto i = std::find_if( wayPoints.begin(), wayPoints.end(), [aWayPoint]( WayPointPtr w)
							   {
									return aWayPoint->getName() == w->getName();
							   });
		if (i != wayPoints.end())
		{
			wayPoints.erase( i);
			if (aNotifyObservers == true)
			{
				notifyObservers();
			}
		}
	}
	/**
	 *
	 */
	void RobotWorld::deleteGoal( 	GoalPtr aGoal,
									bool aNotifyObservers /*= true*/)
	{
		auto i = std::find_if( goals.begin(), goals.end(), [aGoal]( GoalPtr g)
							   {
			return aGoal->getName() == g->getName();
							   });
		if (i != goals.end())
		{
			goals.erase( i);

			if (aNotifyObservers == true)
			{
				notifyObservers();
			}
		}
	}
	/**
	 *
	 */
	void RobotWorld::deleteWall( 	WallPtr aWall,
									bool aNotifyObservers /*= true*/)
	{
		auto i = std::find_if( walls.begin(), walls.end(), [aWall]( WallPtr w)
							   {
			return
							aWall->getPoint1() == w->getPoint1() &&
							aWall->getPoint2() == w->getPoint2();
							   });
		if (i != walls.end())
		{
			walls.erase( i);

			if (aNotifyObservers == true)
			{
				notifyObservers();
			}
		}
	}
	/**
	 *
	 */
	RobotPtr RobotWorld::getRobot( const std::string& aName) const
	{
		for (RobotPtr robot : robots)
		{
			if (robot->getName() == aName)
			{
				return robot;
			}
		}
		return nullptr;
	}
	/**
	 *
	 */
	RobotPtr RobotWorld::getRobot( const Base::ObjectId& anObjectId) const
	{
		for (RobotPtr robot : robots)
		{
			if (robot->getObjectId() == anObjectId)
			{
				return robot;
			}
		}
		return nullptr;
	}
	/**
	 *
	 */
	WayPointPtr RobotWorld::getWayPoint( const std::string& aName) const
	{
		for (WayPointPtr wayPoint : wayPoints)
		{
			if (wayPoint->getName() == aName)
			{
				return wayPoint;
			}
		}
		return nullptr;
	}
	/**
	 *
	 */
	WayPointPtr RobotWorld::getWayPoint( const Base::ObjectId& anObjectId) const
	{
		for (WayPointPtr wayPoint : wayPoints)
		{
			if (wayPoint->getObjectId() == anObjectId)
			{
				return wayPoint;
			}
		}
		return nullptr;
	}
	/**
	 *
	 */
	GoalPtr RobotWorld::getGoal( const std::string& aName) const
	{
		for (GoalPtr goal : goals)
		{
			if (goal->getName() == aName)
			{
				return goal;
			}
		}
		return nullptr;
	}
	/**
	 *
	 */
	GoalPtr RobotWorld::getGoal( const Base::ObjectId& anObjectId) const
	{
		for (GoalPtr goal : goals)
		{
			if (goal->getObjectId() == anObjectId)
			{
				return goal;
			}
		}
		return nullptr;
	}
	/**
	 *
	 */
	WallPtr RobotWorld::getWall( const Base::ObjectId& anObjectId) const
	{
		for (WallPtr wall: walls)
		{
			if (wall->getObjectId() == anObjectId)
			{
				return wall;
			}
		}
		return nullptr;
	}

	/**
	 *
	 */
	const std::vector< RobotPtr >& RobotWorld::getRobots() const
	{
		return robots;
	}
	/**
	 *
	 */
	const std::vector< WayPointPtr >& RobotWorld::getWayPoints() const
	{
		return wayPoints;
	}
	/**
	 *
	 */
	const std::vector< GoalPtr >& RobotWorld::getGoals() const
	{
		return goals;
	}
	/**
	 *
	 */
	const std::vector< WallPtr >& RobotWorld::getWalls() const
	{
		return walls;
	}
	/**
	 *
	 */
	void RobotWorld::populate( int aNumberOfWalls /*= 2*/)
	{
		RobotWorld::getRobotWorld().newRobot( "Robot", Point( 50, 50),false);
		RobotWorld::getRobotWorld().newRobot( "Robot2", Point( 50, 50),false);

		static Point coordinates[] = { Point( 100, 400), Point( 350, 300),
									   Point( 300, 100),
									   Point( 350, 200) };

		for (int i = 0; i < 2 * aNumberOfWalls; i += 2)
		{
			RobotWorld::getRobotWorld().newWall( coordinates[i], coordinates[i + 1],false);
		}

		RobotWorld::getRobotWorld().newGoal( "Goal", Point( 450, 450),false);

		notifyObservers();
	}
	/**
	 *
	 */
	void RobotWorld::unpopulate( bool aNotifyObservers /*= true*/)
	{
		robots.clear();
		wayPoints.clear();
		goals.clear();
		walls.clear();

		if (aNotifyObservers)
		{
			notifyObservers();
		}
	}
	/**
	 *
	 */
	void RobotWorld::unpopulate(const std::vector<Base::ObjectId >& aKeepObjects,
								bool aNotifyObservers /*= true*/)
	{
		if(robots.size()>0)
		{
			robots.erase(	std::remove_if(	robots.begin(),
											robots.end(),
											[&aKeepObjects](RobotPtr aRobot)
											{
											 return std::find(	aKeepObjects.begin(),
																aKeepObjects.end(),
																aRobot->getObjectId()) == aKeepObjects.end();
											}),
							robots.end());
		}
		if(wayPoints.size()>0)
		{
			wayPoints.erase(std::remove_if(	wayPoints.begin(),
											wayPoints.end(),
											[&aKeepObjects](WayPointPtr aWayPoint)
											{
											 return std::find(	aKeepObjects.begin(),
																aKeepObjects.end(),
																aWayPoint->getObjectId()) == aKeepObjects.end();
											}),
							wayPoints.end());
		}
		if(goals.size()>0)
		{
			goals.erase(	std::remove_if(	goals.begin(),
											goals.end(),
											[&aKeepObjects](GoalPtr aGoal)
											{
											 return std::find(	aKeepObjects.begin(),
																aKeepObjects.end(),
																aGoal->getObjectId()) == aKeepObjects.end();
											}),
							goals.end());
		}
		if(walls.size()>0)
		{
			walls.erase(	std::remove_if(	walls.begin(),
											walls.end(),
											[&aKeepObjects](WallPtr aWall)
											{
											 return std::find(	aKeepObjects.begin(),
																aKeepObjects.end(),
																aWall->getObjectId()) == aKeepObjects.end();
											}),
							walls.end());
		}

		if (aNotifyObservers)
		{
			notifyObservers();
		}
	}
	/**
	 *
	 */
	std::string RobotWorld::asString() const
	{
		return ModelObject::asString();
	}
	/**
	 *
	 */
	std::string RobotWorld::asDebugString() const
	{
		std::ostringstream os;

		os << asString() << '\n';

		for( RobotPtr ptr : robots)
		{
			os << ptr->asDebugString() << '\n';
		}
		for( WayPointPtr ptr : wayPoints)
		{
			os << ptr->asDebugString() << '\n';
		}
		for( GoalPtr ptr : goals)
		{
			os << ptr->asDebugString() << '\n';
		}
		for( WallPtr ptr : walls)
		{
			os << ptr->asDebugString() << '\n';
		}

		return os.str();
	}
	/**
	 *
	 */
	RobotWorld::RobotWorld() : localPort("12345"), remotePort("12346"), communicating(false), pointer(this)
	{
	}
	/**
	 *
	 */
	RobotWorld::~RobotWorld()
	{
		// No notification while I am in the destruction mode!
		disableNotification();
		unpopulate();

		if(communicating)
		{
			stopCommunicating();
		}
	}

	//----- ----- [Communication] ----- -----//
	/**
	 *
	 */
	void RobotWorld::startCommunicating()
	{
		if(!communicating)
		{
			communicating = true;


			localPort = "12345";
			if (Application::MainApplication::isArgGiven( "-local_port"))
			{
				localPort = Application::MainApplication::getArg( "-local_port").value;
			}

			Messaging::CommunicationService::getCommunicationService().runRequestHandler( Model::RobotWorld::getRobotWorld().getPointer(),
																						  std::stoi(localPort));
		}
	}

	/**
	 *
	 */
	void RobotWorld::stopCommunicating()
	{
		if(communicating)
		{
			communicating = false;

			localPort = "12345";
			if (Application::MainApplication::isArgGiven( "-local_port"))
			{
				localPort = Application::MainApplication::getArg( "-local_port").value;
			}

			Messaging::Client c1ient( 	"localhost",
										localPort,
										Model::RobotWorld::getRobotWorld().getPointer());
			Messaging::Message message( 1, "stop");
			c1ient.dispatchMessage( message);
		}
	}

	/**
	 *
	 */
	void RobotWorld::handleRequest( Messaging::Message& aMessage)
	{
		switch(aMessage.getMessageType())
		{
			case EchoRequest:
			{
				Application::Logger::log( __PRETTY_FUNCTION__ + std::string(": EchoRequest"));

				aMessage.setMessageType(EchoResponse);
				aMessage.setBody( ": case 1 " + aMessage.asString());
				break;
			}
			case UpdatePositionRequest:
			{
				Application::Logger::log("Request: " + aMessage.getBody());
				auto r2 = RobotWorld::getRobotWorld().getRobot("Robot2");
				r2->setPosition(stringToLocation(aMessage.getBody()));
				break;
			}
			default:
			{
				Application::Logger::log( __PRETTY_FUNCTION__ + std::string(": default"));

				aMessage.setBody( " default  Goodbye cruel world!");
				break;
			}
		}
	}

	/**
	 *
	 */
	void RobotWorld::handleResponse( const Messaging::Message& aMessage)
	{
		switch(aMessage.getMessageType())
		{
			case EchoResponse:
			{
				std::cout << __PRETTY_FUNCTION__ + std::string( ": case EchoResponse: not implemented, ") + aMessage.asString() << std::endl;

				break;
			}
			case UpdatePositionResponse:
			{
				Application::Logger::log("Response: " + aMessage.getBody());
				break;
			}
			default:
			{
				std::cout << __PRETTY_FUNCTION__ + std::string( ": default not implemented, ") + aMessage.asString() << std::endl;
				break;
			}
		}
	}

	Point RobotWorld::stringToLocation(std::string aString)
	{
		unsigned short locationX = (aString[0] - 48) * 1000 + (aString[1] - 48) * 100 + (aString[2] - 48) * 10 + (aString[3] - 48);
		unsigned short locationY = (aString[4] - 48) * 1000 + (aString[5] - 48) * 100 + (aString[6] - 48) * 10 + (aString[7] - 48);
		std::cout << "LOC X: " << locationX << " LOC Y: " << locationY << std::endl;

		return Point(locationX, locationY);
	}

	RobotWorldPtr RobotWorld::getPointer()
	{
		return pointer;
	}

} // namespace Model
