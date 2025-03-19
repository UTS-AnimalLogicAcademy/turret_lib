# turret_lib

This library, **turret_lib**, provides a turretClient and turretLogger class, which communicate with a [**turret-server**](https://github.com/UTS-AnimalLogicAcademy/turret_server), and can be included in any c++ plugin which needs to resolve shotgun tank uris.  

UTSALA provides two example projects which use **turret_lib**: [turret_usd](https://github.com/UTS-AnimalLogicAcademy/turret_usd) and [turref_klf](https://github.com/UTS-AnimalLogicAcademy/turret_klf).  

## Usage

### Examples

An example of a successfully resolved shotgun tank URI would be:
URI:`tank:/s118/maya_publish_asset_cache_usd?Step=model&Task=model&asset_type=setPiece&version=latest&Asset=building01`
Path:`/mnt/ala/mav/2018/jobs/s118/assets/setPiece/building01/model/model/caches/usd/building01_model_model_usd.v045.usd`

### Environment Variables

Turret allows some basic settings to be overriden via environment variables

 * `TURRET_SESSION_ID`
 * `TURRET_SERVER_IP`
 * `TURRET_SERVER_PORT`
 * `TURRET_TIMEOUT`
 * `TURRET_RETRIES`
 * `DEBUG_LOG_LEVEL`
 * `DEBUG_ENABLED`

## Contributing
We use turret across almost every aspect of our USD pipeline and are constantly fixing bugs and finding time to improve turret more and more. We are however, very open to external pull-requests, and growing turret into a more versatile and robust piece of software with your help. Feel free to get in contact directly or through these GitHub repos. We'd love to talk! 

