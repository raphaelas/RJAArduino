import telegram
from dotenv import load_dotenv
import os

load_dotenv()
bot = telegram.Bot(token=os.getenv('TELEGRAM_TOKEN'))
updates = bot.getUpdates()
print(updates[0]['message']['text'])


