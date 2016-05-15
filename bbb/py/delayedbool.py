class DelayedBool:
  """
  params: 
    numTrue:  number of true values necessary before returning true
  """
  def __init__(self, numTrue):
    self.numTrue = numTrue
    self.count = 0

  """
	  Check if the result should be true, given the boolean value
	  numTrue or more sequential true values are needed to return true
	  (true values must be sequential)
	"""
  def check(self, value):
    if value:
      self.count += 1
    else:
      self.count = 0
    return self.count >= self.numTrue

#Validate the functionality
if __name__ == "__main__":
  test = DelayedBool(5)
  print "True 1: ",test.check(True)
  print "True 2: ",test.check(True)
  print "True 3: ",test.check(True)
  print "True 4: ",test.check(True)
  print "True 5: ",test.check(True)
  print "True 6: ",test.check(True)
  print "False : ",test.check(False)
  print "True 1: ",test.check(True)
  print "False : ",test.check(False)
  print "True 2: ",test.check(True)
  print "True 3: ",test.check(True)
  print "True 4: ",test.check(True)
  print "True 5: ",test.check(True)
  print "True 6: ",test.check(True)