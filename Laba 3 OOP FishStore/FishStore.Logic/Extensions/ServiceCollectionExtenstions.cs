using FishStore.Logic.Interfaces.Repositories;
using FishStore.Logic.Interfaces.Services;
using FishStore.Logic.Repositories;
using FishStore.Logic.Services;
using Microsoft.Extensions.DependencyInjection;

namespace FishStore.Logic.Extensions
{
    public static class ServiceCollectionExtenstions
    {
        public static void AddLogicServices(this IServiceCollection services)
        {
            services.AddSingleton<ICenterService, CenterService>();
            services.AddSingleton<IUserService, UserService>();

            services.AddSingleton<ICenterRepository, CenterRepository>();
            services.AddSingleton<IUserRepository, UserRepository>();
        }
    }
}
