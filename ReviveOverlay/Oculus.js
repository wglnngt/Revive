
function generateManifest(manifest) {
    // TODO: Find an executable ending with -Shipping.exe in manifest["files"].
    var launch = '/' + manifest["launchFile"];
    var revive = {
        "applications": [
            {
                "app_key" : appKeyPrefix + manifest["canonicalName"],

                "launch_type" : "binary",
                "binary_path_windows" : "ReviveInjector_x64.exe",
                "arguments" : "Software/" + manifest["canonicalName"] + launch,

                "image_path" : "StoreAssets/" + manifest["canonicalName"] + "/cover_landscape_image.jpg",

                "strings" : {
                    "en_us" : {
                        "name" : manifest["canonicalName"]
                    }
                }
            }
        ]
    };
    ReviveManifest.addManifest(manifest["canonicalName"], JSON.stringify(revive));
}

function loadManifest(manifestURL) {
    var assetsFolder = baseURL + "Software/StoreAssets/";

    var xhr = new XMLHttpRequest;
    xhr.onreadystatechange = function() {
        if (xhr.readyState == XMLHttpRequest.DONE) {
            var manifest = JSON.parse(xhr.responseText);

            // Assume only games have asset bundles and include their cover.
            if (manifest["packageType"] == "ASSET_BUNDLE") {
                var cover = assetsFolder + manifest["canonicalName"] + "/cover_square_image.jpg";
                coverModel.append({coverURL: cover, appKey: manifest["canonicalName"]});
            }

            // Add the application manifest to the Revive manifest.
            if (manifest["packageType"] == "APP" && !manifest["isCore"]) {
                if (!ReviveManifest.loadManifest(manifest["canonicalName"]))
                    generateManifest(manifest);
            }
        }
    }
    xhr.open('GET', manifestURL);
    xhr.send();
}
