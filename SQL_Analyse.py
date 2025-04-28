import sqlite3
import pandas as pd

# Opens the database
def connect_db(db_path):
    db = sqlite3.connect(db_path)
    return db

# Gets events from the database
def fetch_events(db):
    query = "SELECT * FROM pick_events"
    data = pd.read_sql_query(query, db)
    return data

# Gets runs from the database
def fetch_runs(db):
    query = "SELECT * FROM runs"
    data = pd.read_sql_query(query, db)
    return data

# Gets total picks and picks per color
def get_picks(events_data):
    stats = {}
    stats['total_events'] = len(events_data)

    stats['events_per_color'] = events_data['color'].value_counts().to_dict()

    # Get the average pick success rate for all runs
    if 'pick_events' in events_data.columns:
        pickup_counts = events_data['pickup'].value_counts()
        total_pickup = pickup_counts.get(1, 0)
        stats['pickup_succes_rate'] = total_pickup / len(events_data) if len(events_data) else 0

    print("\n=== PICkS ===")
    for key, value in stats.items():
        print(f"key{key}: {value}")

# Finds average time for pick
def get_average_time(events_data):
    events_data['timeStamp'] = pd.to_datetime(events_data['timeStamp'])

    events_data = events_data.sort_values(['run_id', 'timeStamp'])

    success_averages = []
    failed_averages = []
    overall_averages = []

    run_stats = []

    grouped = events_data.groupby('run_id')

    # Find Average for a single run
    for run_id, group in grouped:
        group = group.sort_values('timeStamp')

        if len(group) < 2: continue

        group['time_diff'] = group['timeStamp'].diff().dt.total_seconds()
        group = group.dropna(subset=['time_diff'])

        if group.empty: continue

        success = group[group['pickup'] == 1]
        failed = group[group['pickup'] == 0]

        success_avg = success['time_diff'].mean() if not success.empty else None
        failed_avg = failed['time_diff'].mean() if not failed.empty else None
        overall_avg = group['time_diff'].mean()

        if success_avg is not None:
            success_averages.append(success_avg)
        if failed_avg is not None:
            failed_averages.append(failed_avg)
        if overall_averages is not None:
            overall_averages.append(overall_avg)

        run_stats.append({
            'run_id': run_id,
            'success_avg': success_avg,
            'failed_avg': failed_avg,
            'overall_avg': overall_avg
        })

    final_success_avg = sum(success_averages) / len(success_averages) if success_averages else float('nan')
    final_failed_avg = sum(failed_averages) / len(failed_averages) if failed_averages else float('nan')
    final_overall_avg = sum(overall_averages) / len(overall_averages) if overall_averages else float('nan')

    # Print Average time for specifik run
    print("\n=== AVERAGE PICK TIME per RUN (seconds) ===")
    for stat in run_stats:
        success_str = f"{stat['success_avg']:.2f}s" if stat['success_avg'] is not None else "N/A"
        failed_str = f"{stat['failed_avg']:.2f}s" if stat['failed_avg'] is not None else "N/A"
        overall_str = f"{stat['overall_avg']:.2f}s" if stat['overall_avg'] is not None else "N/A"

        print(f"Run {stat['run_id']}: "
            f"Success Avg: {success_str}, "
            f"Failed Avg: {failed_str}, "
            f"Overall Avg: {overall_str}")
        
    # Print Average time for all runs
    print("\n=== AVERAGE PICK TIME (seconds)===")
    print(f"Successful Picks: {final_success_avg:.2f} seconds" if success_averages else "No successful picks data")
    print(f"Failed Picks: {final_failed_avg:.2f} seconds" if failed_averages else "No failed picks data")
    print(f"Overall Picks: {final_overall_avg:.2f} seconds" if overall_averages else "No picks data")

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
    events_data = fetch_events(db)

    print("=== RUNS ===")
    print(runs_data)

    get_picks(events_data)
    get_average_time(events_data)
    get_success_rate(events_data)

    db.close()

if __name__ == "__main__":
    db_path = "data_log.db"
    main(db_path)
