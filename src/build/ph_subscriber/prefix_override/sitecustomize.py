import sys
if sys.prefix == '/usr':
    sys.real_prefix = sys.prefix
    sys.prefix = sys.exec_prefix = '/home/habiba/AP25_PH_Sensor/src/install/ph_subscriber'
