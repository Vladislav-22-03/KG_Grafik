using System.ComponentModel.DataAnnotations;

namespace Laba_3_OOP_FishStore.Features.DtoModels.Center
{
    public sealed record CenterDto
    {
        [Key]
        public Guid IsnNode { get; init; }

        [Required, MaxLength(20)]
        public string Code { get; init; }

        [Required, MaxLength(255)]
        public string Name { get; init; }
    }
}
