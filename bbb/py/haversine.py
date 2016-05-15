import math

earth_radius = 6371000 #Radius of the earth in meters

#Returns the straight-line distance around the earth between two points.
#Returns the distance in meters
def distance(fromLoc, toLoc):
  lat1 = math.radians(fromLoc[0])
  lat2 = math.radians(toLoc[0])
  deltaLat = math.radians(toLoc[0]-fromLoc[0])
  deltaLon = math.radians(toLoc[1]-fromLoc[1])

  a = math.sin(deltaLat/2) * math.sin(deltaLat/2) + math.cos(lat1) * math.cos(lat2) * math.sin(deltaLon/2) * math.sin(deltaLon/2)

  c = 2 * math.atan2(math.sqrt(a), math.sqrt(1-a))
  
  return earth_radius * c

#Returns the heading between two lat,lon points in radians where 0 is north, and increases clockwise
def heading(fromLoc, toLoc):
  #convert all lat, long values to radians
  lat1 = math.radians(fromLoc[0])
  lon1 = math.radians(fromLoc[1])
  lat2 = math.radians(toLoc[0])
  lon2 = math.radians(toLoc[1])
  y = math.sin(lon2-lon1) * math.cos(lat2)
  x = math.cos(lat1)*math.sin(lat2) - math.sin(lat1)*math.cos(lat2)*math.cos(lon2-lon1)
  return math.atan2(y, x)

#quick test of the functions
if __name__ == '__main__' :
  testPoints = [(42.27460333, -71.811687),
                (42.274176, -71.8120965),
                (42.273733, -71.81242),
                (42.2734715, -71.811949),
                (42.2737871667, -71.8113955)]
  toPoint = (42.274015, -71.811769)
  for fromPoint in testPoints:
    print "Distance: ", distance(fromPoint, toPoint)
    print 'Heading: ', heading(fromPoint, toPoint)


