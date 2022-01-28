from datetime import datetime
from datetime import timedelta

now = tomorrow = datetime.now()
if now.hour >= 8:
    tomorrow = (now + timedelta(days=1))
tomorrowDay = tomorrow.day
tomorrowMonth = tomorrow.month
tomorrowYear = tomorrow.year
timeUntilNextWakeup = datetime(tomorrowYear, tomorrowMonth, tomorrowDay, 7, 58, 0) - datetime.now()
millisecondsUntilNextWakeup = timeUntilNextWakeup.seconds * 1000
print(millisecondsUntilNextWakeup, end='')
