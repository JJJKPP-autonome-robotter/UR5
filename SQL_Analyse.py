import sqlite3
import pandas as pd

def connect_db(db_path):
    db = sqlite3.connect(db_path)
    return db

def fetch_evenets(db):
    query = "SELECT * FROM pick_events"
    data = pd.read_sql_query(query, db)
    return data

def fetch_runs(db):
    query = "SELECT * FROM runs"
    data = pd.read_sql_query(query, db)
    return data

def get_picks(events_data):
    stats = {}
    stats['total_events'] = len(events_data)

    stats['events_per_color'] = events_data['color'].value_counts().to_dict()

    if 'pick_events' in events_data.columns:
        pickup_counts = events_data['pickup'].value_counts()
        total_pickup = pickup_counts.get(1, 0)
        stats['pickup_succes_rate'] = total_pickup / len(events_data) if len(events_data) else 0

    print("\n=== PICkS ===")
    for key, value in stats.items():
        print(f"key{key}: {value}")

def get_average_time(events_data):
    events_data['timeStamp'] = pd.to_datetime(events_data['timeStamp'])

    events_data = events_data.sort_values('timeStamp')

    events_data['time_diff'] = events_data['timeStamp'].diff().dt.total_seconds()

    success = events_data[events_data['pickup'] == 1]
    success = success.sort_values('timeStamp')
    success['time_diff'] = success['timeStamp'].diff().dt.total_seconds()

    failed = events_data[events_data['pickup'] == 0]
    failed = failed.sort_values('timeStamp')
    failed['time_diff'] = failed['timeStamp'].diff().dt.total_seconds()

    overall_avg = events_data['time_diff'].mean()
    success_avg = success['time_diff'].mean()
    failed_avg = failed['time_diff'].mean()

    print("\n=== AVERAGE PICK TIMES (seconds) ===")
    print(f"Successful Picks: {success_avg:.2f} seconds")
    print(f"Failed Picks: {failed_avg:.2f} seconds")
    print(f"Overall Picks: {overall_avg:.2f} seconds")

def get_success_rate(events_data):
    if 'pickup' in events_data.columns:
        pickup_counts = events_data['pickup'].value_counts()
        total_pickup = pickup_counts.get(1, 0)
        succes_rate = total_pickup / len(events_data) if len(events_data) else 0
        
        print("\n=== SUCCESS RATE ===")
        print(f"Success Rate: {succes_rate*100:.2f}%")
    
    if 'color' in events_data.columns:
        color_grouped = events_data.groupby('color')
        succes_rates = color_grouped['pickup'].value_counts(normalize=True).unstack(fill_value=0)

        for color in succes_rates.index:
            success_rate = succes_rates.loc[color].get(1, 0)
            print(f"{color}: {success_rate * 100:.2f}% Success Rate")

def main(db_path):
    db = connect_db(db_path)

    runs_data = fetch_runs(db)
    events_data = fetch_evenets(db)

    print("=== RUNS ===")
    print(runs_data)

    get_picks(events_data)
    get_average_time(events_data)
    get_success_rate(events_data)

    db.close()

if __name__ == "__main__":
    db_path = "data_log.db"
    main(db_path)
