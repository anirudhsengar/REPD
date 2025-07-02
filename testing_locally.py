from REPD_Impl import REPD
from autoencoder import AutoEncoder
import pandas as pd
import numpy as np
from scipy.io import arff
from extract_traditional_features import extract_traditional_features

datasets = ["kc1","kc2"]
dataset_settings = {
  "kc1": ["defects", lambda x: 1 if str(x)=="b'true'" else 0 ],
  "kc2": ["problems", lambda x: 1 if str(x)=="b'yes'" else 0 ],
}

all_x = []
all_y = []

for dataset in datasets:
    print(dataset)
    defect_column_name = dataset_settings[dataset][0]
    defect_column_map_function = dataset_settings[dataset][1]

    # Load dataset
    data, meta = arff.loadarff("./data/"+dataset+".arff")

    # Wrap data into a pandas dataframe
    df = pd.DataFrame(data)

    # Filter out anomalous rows often found in these datasets
    # These are rows where metrics are unrealistically low (e.g., loc < 2)
    if 'loc' in df.columns:
        df = df[df['loc'] >= 2]

    #Adjust defects column
    df[defect_column_name] = df[defect_column_name].map(defect_column_map_function)

    #Remove all with missing values
    df = df.dropna()

    #Remove duplicate instances
    df = df.drop_duplicates()

    # Add the datasets together
    all_x.append(df.drop(columns=[defect_column_name]).values.astype(np.float32))
    all_y.append(df[defect_column_name].values)

x = np.concatenate(all_x, axis=0)
y = np.concatenate(all_y, axis=0)

# REPD model training
autoencoder = AutoEncoder([21,10],0.01,100,50)
classifer = REPD(autoencoder)
classifer.fit(x,y)

for i in range(1,11):
    isBuggy = classifer.predict(extract_traditional_features(f"./data/test_files/{i}.cpp"))
    if isBuggy[0]:
        print(f"file {i}.cpp contains a bug.")




