#include "Robot.hpp"
#include <sstream>
#include <ctime>
#include <chrono>
#include "Thread.hpp"
#include "MathUtils.hpp"
#include "Logger.hpp"
#include "Goal.hpp"
#include "WayPoint.hpp"
#include "Wall.hpp"
#include "RobotWorld.hpp"
#include "Shape2DUtils.hpp"
#include "CommunicationService.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "MainApplication.hpp"
#include "LaserDistanceSensor.hpp"
#include "Logger.hpp"

namespace Model
{
	/**
	 *
	 */
	Robot::Robot() :
								name( ""),
								size( DefaultSize),
								position( DefaultPosition),
								front( 0, 0),
								speed( 0.0),
								acting(false),
								driving(false)
	{
		std::shared_ptr< AbstractSensor > laserSensor( new LaserDistanceSensor( this));
		attachSensor( laserSensor);
	}
	/**
	 *
	 */
	Robot::Robot( const std::string& aName) :
								name( aName),
								size( DefaultSize),
								position( DefaultPosition),
								front( 0, 0),
								speed( 0.0),
								acting(false),
								driving(false)
	{
		std::shared_ptr< AbstractSensor > laserSensor( new LaserDistanceSensor( this));
		attachSensor( laserSensor);
	}
	/**
	 *
	 */
	Robot::Robot(	const std::string& aName,
					const Point& aPosition) :
								name( aName),
								size( DefaultSize),
								position( aPosition),
								front( 0, 0),
								speed( 0.0),
								acting(false),
								driving(false)
	{
		std::shared_ptr< AbstractSensor > laserSensor( new LaserDistanceSensor( this));
		attachSensor( laserSensor);
	}
	/**
	 *
	 */
	Robot::~Robot()
	{
		if(driving)
		{
			stopDriving();
		}
		if(acting)
		{
			stopActing();
		}
	}
	/**
	 *
	 */
	void Robot::setName( const std::string& aName,
						 bool aNotifyObservers /*= true*/)
	{
		name = aName;
		if (aNotifyObservers == true)
		{
			notifyObservers();
		}

	}
	/**
	 *
	 */
	Size Robot::getSize() const
	{
		return size;
	}
	/**
	 *
	 */
	void Robot::setSize(	const Size& aSize,
							bool aNotifyObservers /*= true*/)
	{
		size = aSize;
		if (aNotifyObservers == true)
		{
			notifyObservers();
		}
	}
	/**
	 *
	 */
	void Robot::setPosition(	const Point& aPosition,
								bool aNotifyObservers /*= true*/)
	{
		position = aPosition;
		if (aNotifyObservers == true)
		{
			notifyObservers();
		}
	}
	/**
	 *
	 */
	BoundedVector Robot::getFront() const
	{
		return front;
	}
	/**
	 *
	 */
	void Robot::setFront(	const BoundedVector& aVector,
							bool aNotifyObservers /*= true*/)
	{
		front = aVector;
		if (aNotifyObservers == true)
		{
			notifyObservers();
		}
	}
	/**
	 *
	 */
	float Robot::getSpeed() const
	{
		return speed;
	}
	/**
	 *
	 */
	void Robot::setSpeed( float aNewSpeed,
						  bool aNotifyObservers /*= true*/)
	{
		speed = aNewSpeed;
		if (aNotifyObservers == true)
		{
			notifyObservers();
		}
	}
	/**
	 *
	 */
	void Robot::startActing()
	{
		goal = RobotWorld::getRobotWorld().getGoal("Goal");

		if(goal != nullptr)
		{
			acting = true;
			std::thread newRobotThread( [this]{	startDriving(goal);});
			robotThread.swap( newRobotThread);
		}
		else
		{
			Application::Logger::log("No goal found!");
		}
	}
	/**
	 *
	 */
	void Robot::stopActing()
	{
		acting = false;
		driving = false;
		robotThread.join();
	}
	/**
	 *
	 */
	void Robot::startDriving(GoalPtr aGoal)
	{
		driving = true;
		calculateRoute(aGoal);
		drive();
	}

	/**
	 *
	 */
	void Robot::stopDriving()
	{
		driving = false;
	}


	/**
	 *
	 */
	Region Robot::getRegion() const
	{
		Point translatedPoints[] = { getFrontRight(), getFrontLeft(), getBackLeft(), getBackRight() };
		return Region( 4, translatedPoints);
	}
	/**
	 *
	 */
	bool Robot::intersects( const Region& aRegion) const
	{
		Region region = getRegion();
		region.Intersect( aRegion);
		return !region.IsEmpty();
	}
	/**
	 *
	 */
	Point Robot::getFrontLeft() const
	{
		// x and y are pointing to top left now
		int x = position.x - (size.x / 2);
		int y = position.y - (size.y / 2);

		Point originalFrontLeft( x, y);
		double angle = Utils::Shape2DUtils::getAngle( front) + 0.5 * Utils::PI;

		Point frontLeft( (originalFrontLeft.x - position.x) * std::cos( angle) - (originalFrontLeft.y - position.y) * std::sin( angle) + position.x, (originalFrontLeft.y - position.y) * std::cos( angle)
		+ (originalFrontLeft.x - position.x) * std::sin( angle) + position.y);

		return frontLeft;
	}
	/**
	 *
	 */
	Point Robot::getFrontRight() const
	{
		// x and y are pointing to top left now
		int x = position.x - (size.x / 2);
		int y = position.y - (size.y / 2);

		Point originalFrontRight( x + size.x, y);
		double angle = Utils::Shape2DUtils::getAngle( front) + 0.5 * Utils::PI;

		Point frontRight( (originalFrontRight.x - position.x) * std::cos( angle) - (originalFrontRight.y - position.y) * std::sin( angle) + position.x, (originalFrontRight.y - position.y)
						  * std::cos( angle) + (originalFrontRight.x - position.x) * std::sin( angle) + position.y);

		return frontRight;
	}
	/**
	 *
	 */
	Point Robot::getBackLeft() const
	{
		// x and y are pointing to top left now
		int x = position.x - (size.x / 2);
		int y = position.y - (size.y / 2);

		Point originalBackLeft( x, y + size.y);

		double angle = Utils::Shape2DUtils::getAngle( front) + 0.5 * Utils::PI;

		Point backLeft( (originalBackLeft.x - position.x) * std::cos( angle) - (originalBackLeft.y - position.y) * std::sin( angle) + position.x, (originalBackLeft.y - position.y) * std::cos( angle)
		+ (originalBackLeft.x - position.x) * std::sin( angle) + position.y);

		return backLeft;

	}
	/**
	 *
	 */
	Point Robot::getBackRight() const
	{
		// x and y are pointing to top left now
		int x = position.x - (size.x / 2);
		int y = position.y - (size.y / 2);

		Point originalBackRight( x + size.x, y + size.y);

		double angle = Utils::Shape2DUtils::getAngle( front) + 0.5 * Utils::PI;

		Point backRight( (originalBackRight.x - position.x) * std::cos( angle) - (originalBackRight.y - position.y) * std::sin( angle) + position.x, (originalBackRight.y - position.y) * std::cos( angle)
		+ (originalBackRight.x - position.x) * std::sin( angle) + position.y);

		return backRight;
	}
	/**
	 *
	 */
	void Robot::handleNotification()
	{
		//	std::unique_lock<std::recursive_mutex> lock(robotMutex);

		static int update = 0;
		if ((++update % 200) == 0)
		{
			notifyObservers();
		}
	}

	/**
	 *
	 */
	std::string Robot::asString() const
	{
		std::ostringstream os;

		os << "Robot " << name << " at (" << position.x << "," << position.y << ")";

		return os.str();
	}
	/**
	 *
	 */
	std::string Robot::asDebugString() const
	{
		std::ostringstream os;

		os << "Robot:\n";
		os << AbstractAgent::asDebugString();
		os << "Robot " << name << " at (" << position.x << "," << position.y << ")\n";

		return os.str();
	}
	/**
	 *
	 */
	void Robot::drive()
	{
		try
		{
			//TODO Loop met sensor->setOn() in comments
			for (std::shared_ptr< AbstractSensor > sensor : sensors)
			{
				sensor->setOn();
			}

			if (speed == 0.0)
			{
				speed = 10.0;
			}

			unsigned pathPoint = 0;
			while (position.x > 0 && position.x < 500 && position.y > 0 && position.y < 500 && pathPoint < path.size())
			{
				const PathAlgorithm::Vertex& vertex = path[pathPoint+=speed];
				front = BoundedVector( vertex.asPoint(), position);
				position.x = vertex.x;
				position.y = vertex.y;

				if (arrived(goal))
				{
					Application::Logger::log(__PRETTY_FUNCTION__ + std::string(": arrived"));
					notifyObservers();
					break;
				}
				else
				{
					if(collision())
					{
						Application::Logger::log(__PRETTY_FUNCTION__ + std::string(": collision"));
						notifyObservers();
						break;
					}
				}

				notifyObservers();

				std::string localPort = "12345";
				std::string remotePort= "12399";

				if (Application::MainApplication::isArgGiven( "-local_port"))
				{
					localPort = Application::MainApplication::getArg( "-local_port").value;
				}

				if (Application::MainApplication::isArgGiven( "-remote_port"))
				{
					remotePort = Application::MainApplication::getArg( "-remote_port").value;
				}

				std::cout << "Communicating on ip: localhost, port: " << localPort << " Remote port: " << remotePort << std::endl;

				Messaging::Client c1ient( 	"localhost",
											remotePort,
											Model::RobotWorld::getRobotWorld().getPointer());

				//TODO Fix dit met enum
				Messaging::Message message(Model::RobotWorld::UpdatePositionRequest ,locationToString(position));
				c1ient.dispatchMessage( message);


				//TODO Hier staat een sleep?
				std::this_thread::sleep_for( std::chrono::milliseconds(100));

				// this should be the last thing in the loop
				if(driving == false)
				{
					return;
				}
			} // while

			//TODO Loop met sensor->setOff() in comments
			for (std::shared_ptr< AbstractSensor > sensor : sensors)
			{
				//sensor->setOff();
			}
		}
		catch (std::exception& e)
		{
			Application::Logger::log(e.what());
			std::cerr << __PRETTY_FUNCTION__ << ": " << e.what() << std::endl;
		}
		catch (...)
		{
			std::cerr << __PRETTY_FUNCTION__ << ": unknown exception" << std::endl;
		}
	}

	std::string Robot::locationToString(Point aLocation)
	{
		std::stringstream ss;
		ss << std::setw(4) << std::setfill('0') << aLocation.x;
		ss << std::setw(4) << std::setfill('0') << aLocation.y;

		return ss.str();
	}

	/**
	 *
	 */
	void Robot::calculateRoute(GoalPtr aGoal)
	{
		path.clear();
		if (aGoal != nullptr)
		{
			// Turn off logging if not debugging AStar
			Application::Logger::setDisable();

			front = BoundedVector( aGoal->getPosition(), position);
			handleNotificationsFor( astar);
			path = astar.search( position, aGoal->getPosition(), size);
			stopHandlingNotificationsFor( astar);

			Application::Logger::setDisable( false);
		}
		else
		{
			throw(std::logic_error("No goal found"));
		}
	}

	/*
	 *
	 */
	bool Robot::arrived(GoalPtr aGoal)
	{
		if(aGoal != nullptr)
		{
			if(intersects( aGoal->getRegion()))
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			throw(std::logic_error("No goal found"));
		}
	}

	/**
	 * TODO Een robot raakt nooit een muur?
	 */
	bool Robot::collision()
	{
		Point frontLeft = getFrontLeft();
		Point frontRight = getFrontRight();
		Point backLeft = getBackLeft();
		Point backRight = getBackRight();

		const std::vector< WallPtr >& walls = RobotWorld::getRobotWorld().getWalls();
		for (WallPtr wall : walls)
		{
			if (Utils::Shape2DUtils::intersect( frontLeft, frontRight, wall->getPoint1(), wall->getPoint2()) ||
							Utils::Shape2DUtils::intersect( frontLeft, backLeft, wall->getPoint1(), wall->getPoint2())	||
							Utils::Shape2DUtils::intersect( frontRight, backRight, wall->getPoint1(), wall->getPoint2()))
			{
				return true;
			}
		}
		return false;
	}

} // namespace Model
