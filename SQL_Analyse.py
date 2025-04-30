import sqlite3
import numpy as np
import cv2
import pandas as pd
import ast


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
        print(f"{key}: {value}")


# Finds average time for pick
def get_average_time(events_data):
    events_data = events_data.copy()
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


def show_pick_event_details(event_row):
    img_array = np.frombuffer(event_row['image'], dtype=np.uint8)
    img = cv2.imdecode(img_array, cv2.IMREAD_COLOR)

    mask_array = np.frombuffer(event_row['mask'], dtype=np.uint8)
    mask = cv2.imdecode(mask_array, cv2.IMREAD_GRAYSCALE)

    coord_str = event_row['picPos'].strip('()')
    x_str, y_str = map(str.strip, coord_str.split(','))
    center_x = int(x_str)
    center_y = int(y_str)

    if img is not None:
        cv2.circle(img, (center_x, center_y), radius=5, color=(255, 255, 0), thickness=-1)
        cv2.namedWindow('Captured Image', cv2.WINDOW_NORMAL)
        cv2.resizeWindow('Captured Image', 600, 600)
        cv2.imshow('Captured Image', img)
    else:
        print("No image data.")
    
    if mask is not None:
        mask_color = cv2.cvtColor(mask, cv2.COLOR_GRAY2BGR)
        cv2.circle(mask_color, (center_x, center_y), radius=5, color=(255, 255, 0), thickness=-1)
        cv2.namedWindow('Mask', cv2.WINDOW_NORMAL)
        cv2.resizeWindow('Mask', 600, 600)
        cv2.imshow('Mask', mask_color)
    else:
        print("No mask data.")

    hsv_lower = ast.literal_eval(event_row.get('hsvLower', '(0,0,0)'))
    hsv_upper = ast.literal_eval(event_row.get('hsvUpper', '(255,255,255)'))

    print("\n=== HSV RANGE USED ===")
    print(f"Lower: {hsv_lower}")
    print(f"Upper: {hsv_upper}")

    print("\n=== TARGET COORDS")
    print(f"X: {center_x}, Y: {center_y}")

    cv2.waitKey(0)
    cv2.destroyAllWindows()


def show_run_details(events_data):
    while True:
        user_input = input("\nEnter a rund_id to view stats for specific run (or 'q' to quit): ").strip()
        if user_input == 'q': break
        
        if not user_input.isdigit():
            print("Invalid input. Pleas enter a numeric run_id or 'q' to quit.")
            continue
            
        run_id = int(user_input)
        if run_id not in events_data['run_id'].unique():
            print(f"Run ID {run_id} not found.")
            continue
        
        run_events = events_data[events_data['run_id'] == run_id]
        if run_events.empty:
            print("No events found for run_id: " + run_id)
            return
        
        print(f"\n=== STATS FOR RUN ID {run_id} ===")
        get_picks(run_events)
        get_average_time(run_events)
        get_success_rate(run_events)

        color = input("\nEnter color to view failed picks (or press Enter to skip): ").strip().lower()
        if not color:
            break

        failed_picks = run_events[(run_events['color'].str.lower() == color) & (run_events['pickup'] == 0)]
        if not failed_picks.empty:
            print(f"\nFailed picks for color '{color}':")
            print(f"{'ID':<5} {'TimeStamp':<25} {'Color':<10} {'Pickup':<6}")
            print("-" * 50)
            for _, row in failed_picks.iterrows():
                print(f"{row['id']:<5} {row['timeStamp']:<25} {row['color']:<10} {row['pickup']:<6}")
            
            while True:
                pick_input = input("\nEnter pick ID to view image and mask (or 'b' to go back): ").strip()
                if pick_input.lower() == 'b':
                    break

                if not pick_input.isdigit():
                    print("Invalid input. Please enter a numeric pick ID or 'b' to go back.")
                    continue

                pick_id = int(pick_input)
                if pick_id not in failed_picks['id'].values:
                    print(f"Pick ID {pick_id} not found among failed picks.")
                    continue

                event_row = failed_picks[failed_picks['id'] == pick_id].iloc[0]
                show_pick_event_details(event_row)

        else:
            print(f"No failed pick found for color '{color}'")


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

    show_run_details(events_data)

if __name__ == "__main__":
    db_path = "data_log.db"
    main(db_path)
