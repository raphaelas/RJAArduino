from datetime import datetime
from datetime import timedelta

now = tomorrow = datetime.now()
if now.hour >= 8:
    tomorrow = now + timedelta(days=1)
tomorrowWeekday = tomorrow.weekday() 
print(tomorrowWeekday + 1)
tomorrowDay = tomorrow.day
tomorrowMonth = tomorrow.month
tomorrowYear = tomorrow.year
timeUntilNextWakeup = datetime(tomorrowYear, tomorrowMonth, tomorrowDay, 7, 56, 0) - now
millisecondsUntilNextWakeup = int(timeUntilNextWakeup.total_seconds()) * 1000
print(millisecondsUntilNextWakeup)
